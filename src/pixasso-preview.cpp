/* -*- Mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2011 Stefano Facchini <stefano.facchini@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 */

#include "pixasso-preview.h"

#include "pixasso-snippet.h"

#include <cairomm/context.h>
#include <iomanip>


PixassoPreview::PixassoPreview ()
    : area (new Area (*this)),
      Glib::ObjectBase (typeid (PixassoPreview)),
      prop_zoom (*this, "zoom")
{
    setup_preview ();
    clear ();
}

PixassoPreview::PixassoPreview (Glib::RefPtr<PixassoSnippet> &snippet)
    : area (new Area (*this)),
      Glib::ObjectBase (typeid (PixassoPreview)),
      prop_zoom (*this, "zoom")
{
    setup_preview ();
    set_snippet (snippet);
}

#define ZOOM_STEP 2.0

void
PixassoPreview::setup_preview ()
{
    dnd_targets.push_back (Gtk::TargetEntry ("text/uri-list"));

    area->signal_drag_begin ()
        .connect (sigc::mem_fun (*this, &PixassoPreview::on_area_drag_begin));
    area->signal_drag_data_get ()
        .connect (sigc::mem_fun (*this, &PixassoPreview::on_area_drag_data_get));

    scrolled.add (*area);
    scrolled.signal_event ()
        .connect (sigc::mem_fun (*this, &PixassoPreview::on_event_cb));
    property_zoom ().signal_changed ()
        .connect (sigc::mem_fun (*this, &PixassoPreview::on_zoom_cb));
    set_zoom_100 ();

    scrolled.set_vexpand (true);
    scrolled.set_hexpand (true);
    scrolled.set_shadow_type (Gtk::SHADOW_IN);
    scrolled.get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);
    scrolled.show_all ();

    attach (scrolled, 0, 0, 1, 1);

    Gtk::Toolbar *toolbar = new Gtk::Toolbar ();
    toolbar->get_style_context ()->add_class ("inline-toolbar");
    toolbar->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);
    Gtk::ToolButton *tb;
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_OUT));
    toolbar->append (*tb, sigc::bind<double>
                     (sigc::mem_fun (*this, &PixassoPreview::set_zoom_step), 1 / ZOOM_STEP));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_IN));
    toolbar->append (*tb, sigc::bind<double>
                     (sigc::mem_fun (*this, &PixassoPreview::set_zoom_step), ZOOM_STEP));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_100));
    toolbar->append (*tb, sigc::mem_fun (*this, &PixassoPreview::set_zoom_100));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_FIT));
    toolbar->append (*tb, sigc::mem_fun (*this, &PixassoPreview::set_zoom_fit));

    Gtk::ToolItem *ti = new Gtk::ToolItem ();
    ti->add (zoom_label);
    ti->set_margin_left (6);
    toolbar->append (*ti);

    toolbar->show_all ();

    attach (*toolbar, 0, 1, 1, 1);
}

PixassoPreview::~PixassoPreview ()
{
    delete area;
}

void
PixassoPreview::set_snippet (Glib::RefPtr<PixassoSnippet> &snippet)
{
    (*this).snippet = snippet;
    area->drag_source_set (dnd_targets);
    area->queue_resize ();
}

void
PixassoPreview::on_area_drag_begin (const Glib::RefPtr<Gdk::DragContext>& context)
{
    Cairo::RefPtr<Cairo::Surface> surface;
    Cairo::RefPtr<Cairo::Context> cr;
    int width = ceil (snippet->get_width ());
    int height = ceil (snippet->get_height ());

    surface = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32, width , height);
    cr = Cairo::Context::create (surface);
    snippet->render (cr, 1);
    context->set_icon (surface);
}

void
PixassoPreview::on_area_drag_data_get (const Glib::RefPtr<Gdk::DragContext>& /* context */,
                                       Gtk::SelectionData& selection_data,
                                       guint /* info */,
                                       guint /* time */)
{
    gchar *data;
    SnippetExporter *exporter = snippet->get_exporter (PixassoSnippet::EXPORT_EPS_URI);

    if (!(data = exporter->get_data ()))
        return;

    selection_data.set (exporter->get_mime_type (),
                        8,
                        (const guchar*) data,
                        strlen (data));
    g_free (data);
}

void
PixassoPreview::clear ()
{
    snippet.reset ();
    set_zoom_100 ();
    area->drag_source_unset ();
    area->queue_draw ();
}

#define ZOOM_MIN 0.1
#define ZOOM_MAX 10.0

void
PixassoPreview::set_zoom (double zoom)
{
    double real_zoom = CLAMP (zoom, ZOOM_MIN, ZOOM_MAX);
    property_zoom () = real_zoom;
    g_debug ("ZOOM: %f", real_zoom);
    area->queue_resize ();
}

void
PixassoPreview::set_zoom_step (double step)
{
    set_zoom (property_zoom () * step);
}

void
PixassoPreview::set_zoom_100 ()
{
    set_zoom (1);
}

void
PixassoPreview::set_zoom_fit ()
{
    double x_scale_request = scrolled.get_allocated_width () / snippet->get_width ();
    double y_scale_request = scrolled.get_allocated_height () / snippet->get_height ();
    set_zoom (MIN (x_scale_request, y_scale_request));
}

#define ZOOM_STEP_ON_SCROLL 1.2

bool
PixassoPreview::on_event_cb (GdkEvent *e)
{
    GdkEventScroll *event;

    if (e->type != GDK_SCROLL)
        return false;

    event = (GdkEventScroll *) e;

    switch (event->direction) {
    case GDK_SCROLL_UP:
        set_zoom_step (ZOOM_STEP_ON_SCROLL);
	break;
    case GDK_SCROLL_DOWN:
        set_zoom_step (1 / ZOOM_STEP_ON_SCROLL);
        break;
    }

    return true;
}

void
PixassoPreview::on_zoom_cb ()
{
    double zoom = property_zoom ();
    zoom_label.set_text (Glib::ustring::format (std::fixed, std::setprecision (0), zoom * 100) + "%");
}


void
PixassoPreview::Area::get_preferred_width_vfunc (int& minimum_width,
                                                 int& natural_width) const
{
    int w;
    if (parent.snippet)
        w = ceil (parent.snippet->get_width () * parent.property_zoom ());
    else
        w = 0;
    minimum_width = natural_width = w;
}

void
PixassoPreview::Area::get_preferred_height_vfunc (int& minimum_height,
                                                  int& natural_height) const
{
    int h;
    if (parent.snippet)
        h = ceil (parent.snippet->get_height () * parent.property_zoom ());
    else
        h = 0;
    minimum_height = natural_height = h;
}

bool
PixassoPreview::Area::on_draw (const Cairo::RefPtr< Cairo::Context >& cr)
{
    int min_width;
    int min_height;
    int nat_width;
    int nat_height;
    int x;
    int y;

    cr->set_source_rgb (1, 1, 1);
    cr->paint ();

    if (!parent.snippet)
        return true;

    get_preferred_width (min_width, nat_width);
    get_preferred_height (min_height, nat_height);

    x = floor ((get_allocated_width () - min_width) / 2);
    y = floor ((get_allocated_height () - min_height) / 2);

    cr->translate (x, y);
    parent.snippet->render (cr, parent.property_zoom ());

    return true;
}
