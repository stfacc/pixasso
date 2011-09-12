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

#include "pixasso-utils.h"

#include <cairomm/context.h>
#include <iostream>
#include <giomm/file.h>
#include <glibmm.h>
#include <poppler.h>


#define PDF_FILENAME "a.pdf"
#define LATEX_FILENAME "a.tex"
#define LATEX_BODY_FILENAME "b.tex"
#define KEYFILE_FILENAME "snippet.ini"

#define KEYFILE_GROUP "Snippet"
#define KEYFILE_PREAMBLE "preamble"
#define KEYFILE_FONT_SIZE "font-size"
#define KEYFILE_MATH_MODE "math-mode"


class PixassoSnippet::Private {
public:
    Glib::ustring data_dir;
    time_t creation_time;

    Glib::ustring preamble_name;
    Glib::ustring font_size;
    Glib::ustring latex_body;
    Glib::ustring math_mode;

    int format;
    double cached_zoom_factor;
    PopplerPage *poppler_page;
    Cairo::RefPtr<Cairo::Surface> cached_surface;
    
    bool generated;
    void generate ();

    bool remove_data_on_delete;
};

static PixassoSnippet::MathModeMap
create_math_mode_map ()
{
    PixassoSnippet::MathModeMap m;
    m["Display"].prefix = "\\[";
    m["Display"].suffix = "\\]";
    m["Inline"].prefix = "\\(";
    m["Inline"].suffix = "\\)";
    m["Text"].prefix = "";
    m["Text"].suffix = "";
    return m;
}

PixassoSnippet::MathModeMap
PixassoSnippet::math_mode_map = create_math_mode_map ();

static PopplerPage *poppler_page_get_first_from_file (Glib::ustring path);


// Create a PixassoSnippet from various latex data
PixassoSnippet::PixassoSnippet (Glib::ustring preamble_name,
                                Glib::ustring font_size,
                                Glib::ustring math_mode,
                                Glib::ustring latex_body)
    : priv (new Private ())
{
    gchar *data_dir_cstr;
    Glib::ustring filename;
    Glib::KeyFile keyfile;

    g_debug ("PixassoSnippet: creating snippet from data");

    priv->remove_data_on_delete = false;

    data_dir_cstr = g_build_filename (g_get_user_data_dir (), PACKAGE, "XXXXXX", NULL);
    if (!mkdtemp (data_dir_cstr)) {
        throw std::runtime_error ("PixassoSnippet: cache directory cannot be created");
    } else {
        g_debug ("PixassoSnippet: creating cache directory %s", data_dir_cstr);
    }
    priv->data_dir = Glib::ustring (data_dir_cstr);
    g_free (data_dir_cstr);

    if (preamble_name != "default") {
        throw std::runtime_error ("PixassoSnippet: preamble '" + preamble_name + "' does not exist");
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

    filename = Glib::build_filename (priv->data_dir, KEYFILE_FILENAME);
    Glib::file_set_contents (filename, keyfile.to_data ());

    priv->cached_zoom_factor = -1;

    priv->generate ();
}

// Create a PixassoSnippet from a directory name
PixassoSnippet::PixassoSnippet (Glib::ustring dir_name)
    : priv (new Private ())
{
    Glib::ustring filename;
    Glib::KeyFile keyfile;

    g_debug ("PixassoSnippet: creating snippet from directory %s", dir_name.c_str ());

    priv->remove_data_on_delete = false;

    priv->data_dir = dir_name;

    filename = Glib::build_filename (priv->data_dir, LATEX_BODY_FILENAME);
    priv->latex_body = Glib::file_get_contents (filename);

    filename = Glib::build_filename (priv->data_dir, KEYFILE_FILENAME);
    keyfile.load_from_file (filename, Glib::KEY_FILE_NONE);
    priv->preamble_name = keyfile.get_string (KEYFILE_GROUP, KEYFILE_PREAMBLE);
    priv->font_size = keyfile.get_string (KEYFILE_GROUP, KEYFILE_FONT_SIZE);
    priv->math_mode = keyfile.get_string (KEYFILE_GROUP, KEYFILE_MATH_MODE);

    filename = Glib::build_filename (priv->data_dir, PDF_FILENAME);
    priv->poppler_page = poppler_page_get_first_from_file (filename);

    // FIXME: do we always expect to find a PDF cached file?
    if (priv->poppler_page)
        priv->generated = true;
    else
        throw std::runtime_error ("PixassoSnippet: poppler_page is NULL");
}

PixassoSnippet::~PixassoSnippet ()
{
    if (priv->remove_data_on_delete) {
        g_debug ("PixassoSnippet: deleting %s", priv->data_dir.c_str ());
        Pixasso::remove_dir (priv->data_dir);
    }

    delete priv;
}
    
void
PixassoSnippet::set_remove_data_on_delete (bool remove)
{
    priv->remove_data_on_delete = remove;
}

#define RESOLUTION_SCALE (96.0 / 72.0)

double
PixassoSnippet::get_width ()
{
    double width;
    poppler_page_get_size (priv->poppler_page, &width, NULL);
    return RESOLUTION_SCALE * width;
}

double
PixassoSnippet::get_height ()
{
    double height;
    poppler_page_get_size (priv->poppler_page, NULL, &height);
    return RESOLUTION_SCALE * height;
}

void
PixassoSnippet::render (Cairo::RefPtr<Cairo::Context> cr, double zoom_factor)
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
        context->set_source_rgb (1, 1, 1);
        context->paint ();
        context->scale (real_scale, real_scale);
        poppler_page_render (priv->poppler_page, context->cobj ());

        priv->cached_zoom_factor = zoom_factor;
    }

    cr->set_source (priv->cached_surface, 0, 0);
    cr->paint ();
}

void
PixassoSnippet::set_export_format ()
{
}

time_t
PixassoSnippet::get_creation_time ()
{
    return priv->creation_time;
}

Glib::ustring
PixassoSnippet::get_data_dir ()
{
    return priv->data_dir;
}

Glib::ustring
PixassoSnippet::get_preamble_name ()
{
    return priv->preamble_name;
}

Glib::ustring
PixassoSnippet::get_font_size ()
{
    return priv->font_size;
}

Glib::ustring
PixassoSnippet::get_math_mode ()
{
    return priv->math_mode;
}

Glib::ustring
PixassoSnippet::get_latex_body ()
{
    return priv->latex_body;
}

bool
PixassoSnippet::is_generated ()
{
    return priv->generated;
}

void
PixassoSnippet::Private::generate ()
{
    Glib::ustring latex_full;
    Glib::ustring source_path;
    Glib::RefPtr<Gio::File> source_file;

    if (generated)
        return;
    
    if (preamble_name == "default") {
        latex_full =
            "\\documentclass{article}"
            "\\pagestyle{empty}"
            "\\usepackage{amsmath,amssymb,amsfonts}"
            "\\usepackage{fix-cm}"
            "\\begin{document} "
            "\\fontsize{" + font_size + "}{" + font_size + "}\\selectfont" +
            math_mode_map[math_mode].prefix +
            latex_body +
            math_mode_map[math_mode].suffix +
            "\\end{document}";
    }

    Glib::file_set_contents (LATEX_FILENAME, latex_full);
    
    g_spawn_command_line_sync ("pdflatex -halt-on-error '\\input " LATEX_FILENAME "'", NULL, NULL, NULL, NULL);
    g_spawn_command_line_sync ("pdfcrop " PDF_FILENAME " " PDF_FILENAME, NULL, NULL, NULL, NULL);
    
    source_path = Glib::build_filename (Glib::get_current_dir (), PDF_FILENAME);

    poppler_page = poppler_page_get_first_from_file (source_path);

    if (!poppler_page)
        throw std::runtime_error ("PixassoSnippet: poppler_page is NULL");

    source_file = Gio::File::create_for_path (source_path);
    source_file->move (Gio::File::create_for_path (Glib::build_filename (data_dir, PDF_FILENAME)),
                       Gio::FILE_COPY_NONE);

    generated = true;
}

static PopplerPage *
poppler_page_get_first_from_file (Glib::ustring path)
{
    PopplerDocument *doc;
    PopplerPage *page;
    GError *error = NULL;
    Glib::ustring fileuri = Glib::filename_to_uri (path);

    doc = poppler_document_new_from_file (fileuri.c_str (), NULL, &error);

    if (error) {
        g_critical ("PixassoSnippet: error in poppler_page_get_first_from file: %s\n", error->message);
        g_error_free (error);
        return NULL;
    }

    page = poppler_document_get_page (doc, 0);
    g_object_unref (doc);

    return page;
}
