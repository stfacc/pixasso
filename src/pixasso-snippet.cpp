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


#define PDF_BASENAME "a"
#define PDF_FILENAME "a.pdf"

class PixassoSnippet::Private {
public:
    Glib::ustring preamble_name;
    Glib::ustring latex_body;
    time_t creation_time;
    int format;
    char *data_dir;
    LatexStyle style;

    double cached_zoom_factor;
    PopplerPage *poppler_page;
    Cairo::RefPtr<Cairo::Surface> cached_surface;
    
    bool generated;
    void generate ();

    static const char *style_prefix[];
    static const char *style_suffix[];
};

const char *PixassoSnippet::Private::style_prefix[] = { "\\[",   // DISPLAY
                                                        "$",     // INLINE
                                                        ""       // TEXT
                                                      };

const char *PixassoSnippet::Private::style_suffix[] = { "\\]",   // DISPLAY
                                                        "$",     // INLINE
                                                        ""       // TEXT
                                                      };

const char *PixassoSnippet::LatexStyleLabel[] = { "Display", "Inline", "Text" };

// Create a PixassoSnippet from various latex data
PixassoSnippet::PixassoSnippet (Glib::ustring p,
                                Glib::ustring l,
                                LatexStyle style)
    : priv (new Private ())
{
    gchar *s;

    if (p != "default")
        g_error ("Preamble %s does not exist", p.data ());

    s = g_build_filename (g_get_user_data_dir (), PACKAGE, "XXXXXX", NULL);

    if (!mkdtemp (s))
	g_error ("Cache directory cannot be created");

    priv->data_dir = s;
    priv->preamble_name = p;
    priv->latex_body = l;
    priv->style = style;
    
    priv->cached_zoom_factor = -1;

    //priv->generate ();
}

// Create a PixassoSnippet from a directory name
PixassoSnippet::PixassoSnippet (Glib::ustring d)
{
}

// Remove data directory if it is empty
PixassoSnippet::~PixassoSnippet ()
{
    Glib::RefPtr<Gio::File> f = Gio::File::create_for_path (priv->data_dir);
    try {
        f->remove ();
    } catch (const Glib::Exception& ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }

    g_free (priv->data_dir);
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

void
PixassoSnippet::render (Cairo::RefPtr<Cairo::Context> cr, double zoom_factor)
{
    double real_scale;

    priv->generate ();
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

void
PixassoSnippet::Private::generate ()
{
    Glib::ustring latex_full;

    PopplerDocument *doc;

    GFile *source_file;
    GFile *dest_file;
    
    char *source_path;
    char *dest_path;
    char *s;

    if (generated)
        return;
    
    if (preamble_name == "default") {
        latex_full =
            "'\\documentclass{article}"
            "\\pagestyle{empty}"
            "\\begin{document} " +
            Glib::ustring (style_prefix[style]) +
            latex_body +
            Glib::ustring (style_suffix[style]) +
            "\\end{document}'";
    }

    s = g_strdup_printf ("pdflatex -halt-on-error -jobname " PDF_BASENAME " %s", latex_full.c_str ());
    g_spawn_command_line_sync (s, NULL, NULL, NULL, NULL);
    g_spawn_command_line_sync ("pdfcrop " PDF_FILENAME " " PDF_FILENAME, NULL, NULL, NULL, NULL);
    g_free (s);
    
    source_path = g_build_filename (g_get_current_dir (), PDF_FILENAME, NULL);

    s = g_filename_to_uri (source_path, NULL, NULL);
    doc = poppler_document_new_from_file (s, NULL, NULL);
    poppler_page = poppler_document_get_page (doc, 0);
    g_object_unref (doc);
    g_free (s);
    
    s = g_build_filename (data_dir, PDF_FILENAME, NULL);
    source_file = g_file_new_for_path (source_path);
    dest_file = g_file_new_for_path (s);
    
    g_file_move (source_file, dest_file, G_FILE_COPY_NONE, NULL, NULL, NULL, NULL);

    g_object_unref (source_file);
    g_object_unref (dest_file);    
    g_free (source_path);
    g_free (s);

    generated = true;
}
