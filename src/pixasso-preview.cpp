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

#include <config.h>

#include "pixasso-preview.h"

#include "pixasso-snippet.h"
#include "pixasso-snippet-exporter.h"

#include <cairomm/context.h>
#include <iomanip>
#include <iostream>


using namespace Pixasso;

Preview::Preview ()
    : area (new Area (*this)),
      Glib::ObjectBase (typeid (Preview)),
      prop_zoom (*this, "zoom")
{
    setup_preview ();
    clear ();
}

Preview::Preview (Glib::RefPtr<Snippet> &snippet)
    : area (new Area (*this)),
      Glib::ObjectBase (typeid (Preview)),
      prop_zoom (*this, "zoom")
{
    setup_preview ();
    set_snippet (snippet);
}

#define ZOOM_STEP 2.0

void
Preview::setup_preview ()
{
    dnd_targets.push_back (Gtk::TargetEntry ("text/uri-list"));

    Glib::RefPtr<Gtk::Builder> refBuilder;

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso-preview.ui"));
    } catch (Glib::FileError &e) {
        std::cerr << e.what () << std::endl;
        exit (EXIT_FAILURE);
    }

    area->signal_drag_begin ()
        .connect (sigc::mem_fun (*this, &Preview::on_area_drag_begin));
    area->signal_drag_data_get ()
        .connect (sigc::mem_fun (*this, &Preview::on_area_drag_data_get));

    scrolled.add (*area);
    scrolled.signal_event ()
        .connect (sigc::mem_fun (*this, &Preview::on_event_cb));
    property_zoom ().signal_changed ()
        .connect (sigc::mem_fun (*this, &Preview::on_zoom_cb));

    scrolled.set_vexpand (true);
    scrolled.set_hexpand (true);
    scrolled.set_shadow_type (Gtk::SHADOW_IN);
    scrolled.get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);
    scrolled.show_all ();

    attach (scrolled, 0, 0, 1, 1);

    Gtk::Toolbar *toolbar;
    refBuilder->get_widget ("toolbar", toolbar);
    toolbar->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);

    refBuilder->get_widget ("button-zoom-out", button_zoom_out);
    button_zoom_out->signal_clicked ()
        .connect (sigc::bind<double>
                  (sigc::mem_fun (*this, &Preview::set_zoom_step), 1 / ZOOM_STEP));
    refBuilder->get_widget ("button-zoom-in", button_zoom_in);
    button_zoom_in->signal_clicked ()
        .connect (sigc::bind<double>
                  (sigc::mem_fun (*this, &Preview::set_zoom_step), ZOOM_STEP));
    refBuilder->get_widget ("button-zoom-100", button_zoom_100);
    button_zoom_100->signal_clicked ()
        .connect (sigc::mem_fun (*this, &Preview::set_zoom_100));
    refBuilder->get_widget ("button-zoom-fit", button_zoom_fit);
    button_zoom_fit->signal_clicked ()
        .connect (sigc::mem_fun (*this, &Preview::set_zoom_fit));

    Gtk::ToolItem *ti = new Gtk::ToolItem ();
    ti->add (zoom_label);
    ti->set_margin_left (6);
    toolbar->append (*ti);

    toolbar->show_all ();

    attach (*toolbar, 0, 1, 1, 1);

    set_zoom_100 ();
}

Preview::~Preview ()
{
    delete area;
}

void
Preview::set_snippet (Glib::RefPtr<Snippet> &snippet)
{
    (*this).snippet = snippet;

    button_zoom_out->set_sensitive (true);
    button_zoom_in->set_sensitive (true);
    button_zoom_100->set_sensitive (true);
    button_zoom_fit->set_sensitive (true);
    zoom_label.set_sensitive (true);

    area->drag_source_set (dnd_targets);
    area->queue_resize ();
}

void
Preview::on_area_drag_begin (const Glib::RefPtr<Gdk::DragContext>& context)
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
Preview::on_area_drag_data_get (const Glib::RefPtr<Gdk::DragContext>& /* context */,
                                Gtk::SelectionData& selection_data,
                                guint /* info */,
                                guint /* time */)
{
    SnippetExporter *exporter = SnippetExporterFactory::create (SnippetExporterFactory::EXPORT_EPS_URI,
                                                                snippet);
    gchar *data = exporter->get_data ();

    if (!data)
        return;

    selection_data.set (exporter->get_mime_type (),
                        8,
                        (const guchar*) data,
                        strlen (data));
    g_free (data);
    delete exporter;
}

void
Preview::clear ()
{
    snippet.reset ();

    set_zoom_100 ();
    button_zoom_out->set_sensitive (false);
    button_zoom_in->set_sensitive (false);
    button_zoom_100->set_sensitive (false);
    button_zoom_fit->set_sensitive (false);
    zoom_label.set_sensitive (false);

    area->drag_source_unset ();
    area->queue_draw ();
}

#define ZOOM_MIN 0.1
#define ZOOM_MAX 10.0

void
Preview::set_zoom (double zoom)
{
    double real_zoom = CLAMP (zoom, ZOOM_MIN, ZOOM_MAX);
    property_zoom () = real_zoom;

    button_zoom_out->set_sensitive (real_zoom > ZOOM_MIN);
    button_zoom_in->set_sensitive (real_zoom < ZOOM_MAX);

    area->queue_resize ();
}

void
Preview::set_zoom_step (double step)
{
    set_zoom (property_zoom () * step);
}

void
Preview::set_zoom_100 ()
{
    set_zoom (1);
}

void
Preview::set_zoom_fit ()
{
    double x_scale_request = scrolled.get_allocated_width () / snippet->get_width ();
    double y_scale_request = scrolled.get_allocated_height () / snippet->get_height ();
    set_zoom (MIN (x_scale_request, y_scale_request));
}

#define ZOOM_STEP_ON_SCROLL 1.2

bool
Preview::on_event_cb (GdkEvent *e)
{
    GdkEventScroll *event;

    if (e->type != GDK_SCROLL || !snippet)
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
Preview::on_zoom_cb ()
{
    double zoom = property_zoom ();
    zoom_label.set_text (Glib::ustring::format (std::fixed, std::setprecision (0), zoom * 100) + "%");
}


void
Preview::Area::get_preferred_width_vfunc (int& minimum_width,
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
Preview::Area::get_preferred_height_vfunc (int& minimum_height,
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
Preview::Area::on_draw (const Cairo::RefPtr< Cairo::Context >& cr)
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
