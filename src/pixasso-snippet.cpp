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

#include <cairomm/context.h>
#include <iostream>
#include <giomm/file.h>
#include <glibmm.h>
#include <poppler.h>


#define PDF_FILENAME "a.pdf"
#define LATEX_FILENAME "a.tex"
#define LATEX_BODY_FILENAME "b.tex"

class PixassoSnippet::Private {
public:
    Glib::ustring preamble_name;
    Glib::ustring latex_body;
    time_t creation_time;
    int format;
    Glib::ustring data_dir;
    LatexStyle style;

    double cached_zoom_factor;
    PopplerPage *poppler_page;
    Cairo::RefPtr<Cairo::Surface> cached_surface;
    
    bool generated;
    int generate ();

    static const Glib::ustring style_prefix[];
    static const Glib::ustring style_suffix[];
};

const Glib::ustring
PixassoSnippet::Private::style_prefix[] =
    {
        "\\[",   // DISPLAY
        "$",     // INLINE
        ""       // TEXT
    };

const Glib::ustring
PixassoSnippet::Private::style_suffix[] =
    {
        "\\]",   // DISPLAY
        "$",     // INLINE
        ""       // TEXT
    };

const Glib::ustring
PixassoSnippet::LatexStyleLabel[] =
    {
        "Display",
        "Inline",
        "Text"
    };


static PopplerPage *poppler_page_get_first_from_file (Glib::ustring path);


// Create a PixassoSnippet from various latex data
PixassoSnippet::PixassoSnippet (Glib::ustring preamble_name,
                                Glib::ustring latex_body,
                                LatexStyle style)
    : priv (new Private ())
{
    gchar *data_dir_cstr;
    Glib::ustring filename;

    if (preamble_name != "default")
        g_error ("Preamble %s does not exist", preamble_name.c_str ());
    priv->preamble_name = preamble_name;

    data_dir_cstr = g_build_filename (g_get_user_data_dir (), PACKAGE, "XXXXXX", NULL);
    if (!mkdtemp (data_dir_cstr))
        g_error ("Cache directory cannot be created");
    priv->data_dir = Glib::ustring (data_dir_cstr);
    g_free (data_dir_cstr);

    priv->latex_body = latex_body;
    filename = Glib::build_filename (priv->data_dir, LATEX_BODY_FILENAME);
    try {
        Glib::file_set_contents (filename, priv->latex_body);
    } catch (Glib::Exception &e) {
        g_error ("Error writing " LATEX_BODY_FILENAME);
    }

    priv->style = style;
    priv->cached_zoom_factor = -1;

    priv->generate ();
}

// Create a PixassoSnippet from a directory name
PixassoSnippet::PixassoSnippet (Glib::ustring dir_name)
    : priv (new Private ())
{
    Glib::ustring filename;

    priv->data_dir = dir_name;

    filename = Glib::build_filename (priv->data_dir, LATEX_BODY_FILENAME);
    try {
        priv->latex_body = Glib::file_get_contents (filename);
    } catch (Glib::Exception &e) {
        g_error ("Error reading " LATEX_BODY_FILENAME);
    }

    filename = Glib::build_filename (priv->data_dir, PDF_FILENAME);
    priv->poppler_page = poppler_page_get_first_from_file (filename);

    // FIXME: do we always expect to find a PDF cached file?
    if (priv->poppler_page)
        priv->generated = true;
}

// Remove data directory if it is empty
PixassoSnippet::~PixassoSnippet ()
{
    Glib::RefPtr<Gio::File> f = Gio::File::create_for_path (priv->data_dir);
    try {
        f->remove ();
    } catch (const Glib::Exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    delete priv;
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

int
PixassoSnippet::render (Cairo::RefPtr<Cairo::Context> cr, double zoom_factor)
{
    double real_scale;

    if (!priv->generated)
        return -1;

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
        try {
            poppler_page_render (priv->poppler_page, context->cobj ());
        } catch (...) {
            g_critical ("Poppler couldn't render the PDF");
            return -1;
        }
        priv->cached_zoom_factor = zoom_factor;
    }

    cr->set_source (priv->cached_surface, 0, 0);
    cr->paint ();

    return 0;
}

void
PixassoSnippet::set_export_format ()
{
}

Glib::ustring
PixassoSnippet::get_latex_body ()
{
    return priv->latex_body;
}

Glib::ustring
PixassoSnippet::get_preamble_name ()
{
    return priv->preamble_name;
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

bool
PixassoSnippet::is_generated ()
{
    return priv->generated;
}

int
PixassoSnippet::Private::generate ()
{
    Glib::ustring latex_full;
    Glib::ustring source_path;
    Glib::RefPtr<Gio::File> source_file;

    if (generated)
        return 0;
    
    if (preamble_name == "default") {
        latex_full =
            "\\documentclass{article}"
            "\\pagestyle{empty}"
            "\\begin{document} " +
            style_prefix[style] +
            latex_body +
            style_suffix[style] +
            "\\end{document}";
    }

    try {
        Glib::file_set_contents (LATEX_FILENAME, latex_full);
    } catch (...) {
        g_critical ("Cannot write: " LATEX_FILENAME);
        return -1;
    }
    
    g_spawn_command_line_sync ("pdflatex -halt-on-error '\\input " LATEX_FILENAME "'", NULL, NULL, NULL, NULL);
    g_spawn_command_line_sync ("pdfcrop " PDF_FILENAME " " PDF_FILENAME, NULL, NULL, NULL, NULL);
    
    source_path = Glib::build_filename (Glib::get_current_dir (), PDF_FILENAME);

    poppler_page = poppler_page_get_first_from_file (source_path);

    if (!poppler_page)
        return -1;

    source_file = Gio::File::create_for_path (source_path);
    source_file->move (Gio::File::create_for_path (Glib::build_filename (data_dir, PDF_FILENAME)),
                       Gio::FILE_COPY_NONE);

    generated = true;

    return 0;
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
        g_print ("Error: %s\n", error->message);
        g_error_free (error);
        return NULL;
    }

    page = poppler_document_get_page (doc, 0);
    g_object_unref (doc);

    return page;
}
