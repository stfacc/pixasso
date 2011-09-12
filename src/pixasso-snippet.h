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

#ifndef PIXASSO_SNIPPET_H
#define PIXASSO_SNIPPET_H

#include <glibmm/object.h>
#include <glibmm/ustring.h>
#include <cairomm/context.h>
#include <map>

class PixassoSnippet : public Glib::Object
{
public:
    typedef struct {
        Glib::ustring prefix;
        Glib::ustring suffix;
    } MathMode;

    typedef std::map<Glib::ustring, MathMode> MathModeMap;

    static MathModeMap math_mode_map;

    PixassoSnippet (Glib::ustring, Glib::ustring, Glib::ustring, Glib::ustring);
    PixassoSnippet (Glib::ustring);
    ~PixassoSnippet ();

    Glib::ustring get_data_dir ();
    time_t get_creation_time ();

    Glib::ustring get_preamble_name ();
    Glib::ustring get_font_size ();
    Glib::ustring get_math_mode ();
    Glib::ustring get_latex_body ();

    void set_export_format ();
    void set_remove_data_on_delete (bool);

    double get_width ();
    double get_height ();
    bool is_generated ();
    void render (Cairo::RefPtr<Cairo::Context>, double);
private:
    class Private;
    Private *priv;
};

#endif
