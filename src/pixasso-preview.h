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


class PixassoPreview : public Gtk::Grid {
public:
    PixassoPreview ();
    PixassoPreview (Glib::RefPtr<PixassoSnippet> &);
    ~PixassoPreview ();
    void set_snippet (Glib::RefPtr<PixassoSnippet> &);
    void clear ();

private:
    class Area;
    Area *area;

    Gtk::Label zoom_label;
    void setup_preview ();
    bool on_event_cb (GdkEvent *);
    void on_zoomfactor_cb ();
};

#endif
