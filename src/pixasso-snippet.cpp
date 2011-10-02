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

#include "pixasso-snippet.h"

#include "pixasso-snippet-private.h"
#include "pixasso-utils.h"

#include <cairomm/context.h>
#include <gdkmm/rgba.h>
#include <giomm/file.h>
#include <glib/gi18n.h>
#include <glibmm.h>
#include <iostream>
#include <poppler.h>


#define LATEX_BODY_FILENAME "b.tex"

#define KEYFILE_FILENAME "snippet.conf"

#define KEYFILE_GROUP "Snippet"
#define KEYFILE_CREATION_TIME "creation-time"
#define KEYFILE_PREAMBLE "preamble"
#define KEYFILE_FONT_SIZE "font-size"
#define KEYFILE_COLOR "color"
#define KEYFILE_MATH_MODE "math-mode"

using namespace Pixasso;

static Snippet::MathModeMap
create_math_mode_map ()
{
    Snippet::MathModeMap m;
    m["display"].prefix = "\\[";
    m["display"].suffix = "\\]";
    m["display"].display_id = _("Display");
    m["inline"].prefix = "\\(";
    m["inline"].suffix = "\\)";
    m["inline"].display_id = _("Inline");
    m["text"].prefix = " ";
    m["text"].suffix = " ";
    m["text"].display_id = _("Text");
    return m;
}

Snippet::MathModeMap
Snippet::math_mode_map = create_math_mode_map ();

// Create a Snippet from various latex data
Snippet::Snippet (Glib::ustring preamble_name,
                                Glib::ustring font_size,
                                Gdk::RGBA color,
                                Glib::ustring math_mode,
                                Glib::ustring latex_body)
    : priv (new SnippetPrivate ())
{
    gchar *data_dir_cstr;
    Glib::ustring filename;
    Glib::KeyFile keyfile;

    g_debug ("Snippet: creating snippet from data");

    priv->remove_data_on_delete = false;

    data_dir_cstr = g_build_filename (g_get_user_data_dir (), PACKAGE, "XXXXXX", NULL);
    if (!mkdtemp (data_dir_cstr)) {
        throw std::runtime_error ("Snippet: cache directory cannot be created");
    } else {
        g_debug ("Snippet: creating cache directory %s", data_dir_cstr);
    }
    priv->data_dir = Glib::ustring (data_dir_cstr);
    g_free (data_dir_cstr);

    priv->creation_time.assign_current_time ();
    keyfile.set_string (KEYFILE_GROUP, KEYFILE_CREATION_TIME, priv->creation_time.as_iso8601 ());

    if (preamble_name != "default") {
        throw std::runtime_error ("Snippet: preamble '" + preamble_name + "' does not exist");
    }
    priv->preamble_name = preamble_name;
    keyfile.set_string (KEYFILE_GROUP, KEYFILE_PREAMBLE, preamble_name);

    priv->math_mode = math_mode;
    keyfile.set_string (KEYFILE_GROUP, KEYFILE_MATH_MODE, math_mode);

    priv->latex_body = latex_body;
    filename = Glib::build_filename (priv->data_dir, LATEX_BODY_FILENAME);
    Glib::file_set_contents (filename, priv->latex_body);

    priv->font_size = font_size;
    keyfile.set_string (KEYFILE_GROUP, KEYFILE_FONT_SIZE, font_size);

    priv->color = color;
    keyfile.set_string (KEYFILE_GROUP, KEYFILE_COLOR, color.to_string ());

    filename = Glib::build_filename (priv->data_dir, KEYFILE_FILENAME);
    Glib::file_set_contents (filename, keyfile.to_data ());

    priv->cached_zoom_factor = -1;

    priv->generate ();
}

// Create a Snippet from a directory name
Snippet::Snippet (Glib::ustring dir_name)
    : priv (new SnippetPrivate ())
{
    Glib::ustring filename;
    Glib::KeyFile keyfile;

    g_debug ("Snippet: creating snippet from directory %s", dir_name.c_str ());

    priv->remove_data_on_delete = false;

    priv->data_dir = dir_name;

    filename = Glib::build_filename (priv->data_dir, LATEX_BODY_FILENAME);
    priv->latex_body = Glib::file_get_contents (filename);

    filename = Glib::build_filename (priv->data_dir, KEYFILE_FILENAME);
    keyfile.load_from_file (filename, Glib::KEY_FILE_NONE);
    priv->creation_time.assign_from_iso8601 (keyfile.get_string (KEYFILE_GROUP, KEYFILE_CREATION_TIME));
    priv->preamble_name = keyfile.get_string (KEYFILE_GROUP, KEYFILE_PREAMBLE);
    priv->font_size = keyfile.get_string (KEYFILE_GROUP, KEYFILE_FONT_SIZE);
    priv->color.set (keyfile.get_string (KEYFILE_GROUP, KEYFILE_COLOR));
    priv->math_mode = keyfile.get_string (KEYFILE_GROUP, KEYFILE_MATH_MODE);
    priv->generate_latex_full ();

    filename = priv->get_pdf_path ();
    priv->poppler_page = priv->poppler_page_get_first_from_file (filename);

    // FIXME: do we always expect to find a PDF cached file?
    if (priv->poppler_page)
        priv->generated = true;
    else
        throw std::runtime_error ("Snippet: poppler_page is NULL");
}

Snippet::~Snippet ()
{
    if (priv->remove_data_on_delete) {
        g_debug ("Snippet: deleting %s", priv->data_dir.c_str ());
        ::remove_dir (priv->data_dir);
    }

    delete priv;
}

void
Snippet::set_remove_data_on_delete (bool remove)
{
    priv->remove_data_on_delete = remove;
}

#define RESOLUTION_SCALE (96.0 / 72.0)

double
Snippet::get_width ()
{
    double width;
    poppler_page_get_size (priv->poppler_page, &width, NULL);
    return RESOLUTION_SCALE * width;
}

double
Snippet::get_height ()
{
    double height;
    poppler_page_get_size (priv->poppler_page, NULL, &height);
    return RESOLUTION_SCALE * height;
}

void
Snippet::render (Cairo::RefPtr<Cairo::Context> cr, double zoom_factor)
{
    double real_scale;

    g_return_if_fail (priv->generated);

    // FIXME: poppler_page_render does not honor cairo clipping,
    // so we need a cairo surface as intermediate step

    Cairo::RefPtr<Cairo::Context> context;

    if (zoom_factor != priv->cached_zoom_factor) {
        g_debug ("Regenarating cached surface");
        priv->cached_surface = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32,
                                                            ceil (get_width () * zoom_factor),
                                                            ceil (get_height () * zoom_factor));
        real_scale = RESOLUTION_SCALE * zoom_factor;
        context = Cairo::Context::create (priv->cached_surface);
        context->scale (real_scale, real_scale);
        poppler_page_render (priv->poppler_page, context->cobj ());

        priv->cached_zoom_factor = zoom_factor;
    }

    cr->set_source (priv->cached_surface, 0, 0);
    cr->paint ();
}

Glib::TimeVal
Snippet::get_creation_time ()
{
    return priv->creation_time;
}

Glib::ustring
Snippet::get_data_dir ()
{
    return priv->data_dir;
}

Glib::ustring
Snippet::get_preamble_name ()
{
    return priv->preamble_name;
}

Glib::ustring
Snippet::get_font_size ()
{
    return priv->font_size;
}

Gdk::RGBA
Snippet::get_color ()
{
    return priv->color;
}

Glib::ustring
Snippet::get_math_mode ()
{
    return priv->math_mode;
}

Glib::ustring
Snippet::get_latex_body ()
{
    return priv->latex_body;
}

Glib::ustring
Snippet::get_latex_full ()
{
    return priv->latex_full;
}

bool
Snippet::is_generated ()
{
    return priv->generated;
}

