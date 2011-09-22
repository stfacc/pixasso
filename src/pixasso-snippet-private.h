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

#ifndef PIXASSO_SNIPPET_PRIVATE_H
#define PIXASSO_SNIPPET_PRIVATE_H

#include <cairomm/context.h>
#include <gdkmm/rgba.h>
#include <glibmm.h>
#include <poppler.h>

namespace Pixasso
{

    class SnippetPrivate {
    public:
        Glib::ustring data_dir;
        time_t creation_time;

        Glib::ustring preamble_name;
        Glib::ustring font_size;
        Gdk::RGBA color;
        Glib::ustring latex_body;
        Glib::ustring math_mode;
        Glib::ustring latex_full;

        double cached_zoom_factor;
        PopplerPage *poppler_page;
        Cairo::RefPtr<Cairo::Surface> cached_surface;

        bool generated;
        void generate ();
        void generate_latex_full ();
        PopplerPage *poppler_page_get_first_from_file (Glib::ustring);

        bool remove_data_on_delete;
    };

} // End namespace Pixasso

#endif // PIXASSO_SNIPPET_PRIVATE_H
