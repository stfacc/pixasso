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


class PixassoPreview::Area
    : public Gtk::DrawingArea {
public:
    Glib::RefPtr<PixassoSnippet> snippet;
    double zoom_factor;

protected:
    virtual bool on_draw (const Cairo::RefPtr< Cairo::Context >& cr);
    virtual void get_preferred_width_vfunc (int& minimum_width, int& natural_width) const;
    virtual void get_preferred_height_vfunc (int& minimum_height, int& natural_height) const;
};


PixassoPreview::PixassoPreview ()
    : area (new Area ())
{
    setup_preview ();
    clear ();
}

PixassoPreview::PixassoPreview (Glib::RefPtr<PixassoSnippet> &snippet)
    : area (new Area ())
{
    setup_preview ();
    set_snippet (snippet);
}

void
PixassoPreview::setup_preview ()
{
    Gtk::Widget *widget;

    widget = new Gtk::ScrolledWindow ();
    ((Gtk::ScrolledWindow *) widget)->add (*area);
    widget->signal_event ().connect (sigc::mem_fun (*this,
                                                    &PixassoPreview::on_event_cb));

    widget->set_vexpand (true);
    widget->set_hexpand (true);
    ((Gtk::ScrolledWindow *) widget)->set_shadow_type (Gtk::SHADOW_IN);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);
    widget->show_all ();

    attach (*widget, 0, 0, 1, 1);

    widget = new Gtk::Toolbar ();
    widget->get_style_context ()->add_class ("inline-toolbar");
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);
    Gtk::ToolButton *tb;
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_OUT));
    ((Gtk::Toolbar *) widget)->append (*tb, sigc::mem_fun (*this,
                                                           &PixassoPreview::set_zoom_out));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_IN));
    ((Gtk::Toolbar *) widget)->append (*tb, sigc::mem_fun (*this,
                                                           &PixassoPreview::set_zoom_in));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_100));
    ((Gtk::Toolbar *) widget)->append (*tb, sigc::mem_fun (*this,
                                                           &PixassoPreview::set_zoom_100));
    Gtk::ToolItem *ti = new Gtk::ToolItem ();
    ti->add (zoom_label);
    ti->set_margin_left (6);
    ((Gtk::Toolbar *) widget)->append (*ti);
    
    widget->show_all ();

    attach (*widget, 0, 1, 1, 1);

    set_zoom_100 ();
}

PixassoPreview::~PixassoPreview ()
{
    delete area;
}

void
PixassoPreview::set_snippet (Glib::RefPtr<PixassoSnippet> &snippet)
{
    area->snippet = snippet;
    area->queue_resize ();
}

void
PixassoPreview::clear ()
{
    area->snippet.reset ();
    area->queue_draw ();
}

void
PixassoPreview::set_zoom_factor (double factor)
{
    area->zoom_factor = factor;
    zoom_label.set_text (Glib::ustring::format (std::fixed, std::setprecision (0), factor * 100) + "%");
    area->queue_resize ();
}

#define PIXASSO_PREVIEW_ZOOM_STEP_FACTOR 2
#define PIXASSO_PREVIEW_ZOOM_STEP_FACTOR_SCROLL 1.2

void
PixassoPreview::set_zoom_in ()
{
    set_zoom_factor (get_zoom_factor () * PIXASSO_PREVIEW_ZOOM_STEP_FACTOR);
}

void
PixassoPreview::set_zoom_out ()
{
    set_zoom_factor (get_zoom_factor () / PIXASSO_PREVIEW_ZOOM_STEP_FACTOR);
}

void
PixassoPreview::set_zoom_100 ()
{
    set_zoom_factor (1);
}

double
PixassoPreview::get_zoom_factor ()
{
    return area->zoom_factor;
}

bool
PixassoPreview::on_event_cb (GdkEvent *e)
{
    GdkEventScroll *event;

    if (e->type != GDK_SCROLL)
        return false;
    
    event = (GdkEventScroll *) e;
    
    switch (event->direction) {
    case GDK_SCROLL_UP:
        set_zoom_factor (get_zoom_factor () * PIXASSO_PREVIEW_ZOOM_STEP_FACTOR_SCROLL);
	break;
    case GDK_SCROLL_DOWN:
        set_zoom_factor (get_zoom_factor () / PIXASSO_PREVIEW_ZOOM_STEP_FACTOR_SCROLL);
        break;
    }
    g_debug ("ZOOM: %f", get_zoom_factor ());
    
    return true;
}

void
PixassoPreview::Area::get_preferred_width_vfunc (int& minimum_width,
                                                 int& natural_width) const
{
    int w;
    if (snippet)
        w = ceil (snippet->get_width () * zoom_factor);
    else
        w = 0;
    minimum_width = natural_width = w;
}

void
PixassoPreview::Area::get_preferred_height_vfunc (int& minimum_height,
                                                  int& natural_height) const
{
    int h;
    if (snippet)
        h = ceil (snippet->get_height () * zoom_factor);
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
    
    if (!snippet)
        return true;

    get_preferred_width (min_width, nat_width);
    get_preferred_height (min_height, nat_height);

    x = floor ((get_allocation ().get_width () - min_width) / 2);
    y = floor ((get_allocation ().get_height () - min_height) / 2);
    
    cr->translate (x, y);
    snippet->render (cr, zoom_factor);

    return true;
}
