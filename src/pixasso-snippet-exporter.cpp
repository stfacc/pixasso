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

#include "pixasso-snippet-exporter.h"

#include "pixasso-snippet.h"
#include "pixasso-snippet-private.h"


using namespace Pixasso;

SnippetPrivate *
SnippetExporter::get_snippet_priv ()
{
    return snippet->priv;
}

/*
 * To add a new exporter Foo:
 *  - implement a class SnippetExporterFoo
 *  - add EXPORT_FOO to enum SnippetExporterFactory::ExportFormat
 *  - add a case EXPORT_FOO to SnippetExporterFactory::create
 */

/***********************/
/* Plain Text exporter */
/***********************/

gchar *
SnippetExporterPlainText::get_data ()
{
    return g_strdup (snippet->get_latex_full ().c_str ());
}

/***********************/
/* Eps Uri exporter    */
/***********************/

#define EPS_FILENAME "a.eps"

gchar *
SnippetExporterEpsUri::get_data ()
{
    if (!snippet) {
        g_warning ("SnippetExporter::get_data(): no snippet set");
        return NULL;
    }

    if (!Glib::file_test (Glib::build_filename (snippet->get_data_dir (), EPS_FILENAME), Glib::FILE_TEST_EXISTS))
        generate ();

    Glib::ustring uri;
    uri = Glib::filename_to_uri (Glib::build_filename (snippet->get_data_dir (), EPS_FILENAME));
    return g_strdup (uri.c_str ());
}

void
SnippetExporterEpsUri::generate ()
{
    Glib::ustring cmdline =
        "pdftops -eps " +
        get_snippet_priv ()->get_pdf_path () + " " +
        Glib::build_filename (snippet->get_data_dir (), EPS_FILENAME);
    Glib::spawn_command_line_sync (cmdline);
}


/***********************/
/* Pdf Uri exporter    */
/***********************/

gchar *
SnippetExporterPdfUri::get_data ()
{
    Glib::ustring uri;
    uri = Glib::filename_to_uri (get_snippet_priv ()->get_pdf_path ());
    return g_strdup (uri.c_str ());
}


/***********************/
/* Png Uri exporter    */
/***********************/

#define PNG_FILENAME "a.png"

gchar *
SnippetExporterPngUri::get_data ()
{
    if (!Glib::file_test (Glib::build_filename (snippet->get_data_dir (), PNG_FILENAME), Glib::FILE_TEST_EXISTS))
        generate ();

    Glib::ustring uri;
    uri = Glib::filename_to_uri (Glib::build_filename (snippet->get_data_dir (), PNG_FILENAME));
    return g_strdup (uri.c_str ());
}

void
SnippetExporterPngUri::generate ()
{
    Cairo::RefPtr<Cairo::Surface> surface;
    Cairo::RefPtr<Cairo::Context> cr;
    int width = ceil (snippet->get_width ());
    int height = ceil (snippet->get_height ());

    surface = Cairo::ImageSurface::create (Cairo::FORMAT_ARGB32, width , height);
    cr = Cairo::Context::create (surface);
    snippet->render (cr, 1);
    surface->write_to_png (Glib::build_filename (snippet->get_data_dir (), PNG_FILENAME));
}

SnippetExporter *
SnippetExporterFactory::create (ExportFormat format)
{
    switch (format) {
    case EXPORT_PLAIN_TEXT:
        return new SnippetExporterPlainText ();
    case EXPORT_EPS_URI:
        return new SnippetExporterEpsUri ();
    case EXPORT_PDF_URI:
        return new SnippetExporterPdfUri ();
    case EXPORT_PNG_URI:
        return new SnippetExporterPngUri ();
    default:
        return NULL;
    }
}

SnippetExporter *
SnippetExporterFactory::create (ExportFormat format,
                                Glib::RefPtr<Snippet> snippet)
{
    SnippetExporter *exporter;

    exporter = create (format);
    exporter->set_snippet (snippet);
    return exporter;
}
