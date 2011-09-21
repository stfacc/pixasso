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

#ifndef PIXASSO_PREVIEW_H
#define PIXASSO_PREVIEW_H

#include "pixasso-snippet.h"

#include <gtkmm.h>


namespace Pixasso
{

class Preview
    : public Gtk::Grid
{
    class Area
        : public Gtk::DrawingArea
    {
    public:
        Area (Preview &x) : parent (x) {}

    private:
        Preview &parent;

    protected:
        virtual bool on_draw (const Cairo::RefPtr< Cairo::Context >& cr);
        virtual void get_preferred_width_vfunc (int& minimum_width, int& natural_width) const;
        virtual void get_preferred_height_vfunc (int& minimum_height, int& natural_height) const;
    };

public:
    Preview ();
    Preview (Glib::RefPtr<Snippet> &);
    ~Preview ();
    void set_snippet (Glib::RefPtr<Snippet> &);
    void clear ();

    const Glib::PropertyProxy_ReadOnly<double> property_zoom () const
    { return Glib::PropertyProxy_ReadOnly<double> (this, "zoom"); }

private:
    Glib::PropertyProxy<double> property_zoom ()
    { return prop_zoom.get_proxy (); }

    Area *area;

    Gtk::ScrolledWindow scrolled;

    std::vector<Gtk::TargetEntry> dnd_targets;
    Glib::RefPtr<Snippet> snippet;
    Glib::Property<double> prop_zoom;

    Gtk::Label zoom_label;
    void set_zoom (double);
    void set_zoom_step (double);
    void set_zoom_100 ();
    void set_zoom_fit ();

    void setup_preview ();
    bool on_event_cb (GdkEvent *);
    void on_zoom_cb ();

    void on_area_drag_begin (const Glib::RefPtr< Gdk::DragContext >& context);
    void on_area_drag_data_get (const Glib::RefPtr<Gdk::DragContext>& context,
                                Gtk::SelectionData& selection_data,
                                guint info,
                                guint time);
};

} /* end namespace Pixasso */

#endif
