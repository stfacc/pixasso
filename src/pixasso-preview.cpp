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
    area->queue_resize ();
}

void
PixassoPreview::clear ()
{
    snippet.reset ();
    set_zoom_100 ();
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
