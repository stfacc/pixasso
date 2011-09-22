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

#include "pixasso-snippet.h"
#include "pixasso-snippet-private.h"

#include <giomm/file.h>

#define PDF_FILENAME "a.pdf"
#define LATEX_FILENAME "a.tex"

using namespace Pixasso;

void
SnippetPrivate::generate_latex_full ()
{
    if (preamble_name != "default")
        throw;

    Glib::ustring color_str = Glib::ustring::compose ("%1,%2,%3",
                                                      color.get_red (),
                                                      color.get_green (),
                                                      color.get_blue ());

    latex_full =
        "\\documentclass{article}"
        "\\pagestyle{empty}"
        "\\usepackage{amsmath,amssymb,amsfonts}"
        "\\usepackage{fix-cm}"
        "\\usepackage{color}"
        "\\definecolor{pixasso-color}{rgb}{" + color_str + "}"
        "\\begin{document} "
        "\\fontsize{" + font_size + "}{" + font_size + "}\\selectfont" +
        "\\color{pixasso-color}" +
        Snippet::math_mode_map[math_mode].prefix +
        latex_body +
        Snippet::math_mode_map[math_mode].suffix +
        "\\end{document}";
}

void
SnippetPrivate::generate ()
{
    Glib::ustring source_path;
    Glib::RefPtr<Gio::File> source_file;

    if (generated)
        return;

    generate_latex_full ();

    Glib::file_set_contents (LATEX_FILENAME, latex_full);

    g_spawn_command_line_sync ("pdflatex -halt-on-error '\\input " LATEX_FILENAME "'", NULL, NULL, NULL, NULL);
    g_spawn_command_line_sync ("pdfcrop " PDF_FILENAME " " PDF_FILENAME, NULL, NULL, NULL, NULL);

    source_path = Glib::build_filename (Glib::get_current_dir (), PDF_FILENAME);

    poppler_page = poppler_page_get_first_from_file (source_path);

    if (!poppler_page)
        throw std::runtime_error ("Snippet: poppler_page is NULL");

    source_file = Gio::File::create_for_path (source_path);
    source_file->move (Gio::File::create_for_path (get_pdf_path ()),
                       Gio::FILE_COPY_NONE);

    generated = true;
}

PopplerPage *
SnippetPrivate::poppler_page_get_first_from_file (Glib::ustring path)
{
    PopplerDocument *doc;
    PopplerPage *page;
    GError *error = NULL;
    Glib::ustring fileuri = Glib::filename_to_uri (path);

    doc = poppler_document_new_from_file (fileuri.c_str (), NULL, &error);

    if (error) {
        g_critical ("Snippet: error in poppler_page_get_first_from file: %s\n", error->message);
        g_error_free (error);
        return NULL;
    }

    page = poppler_document_get_page (doc, 0);
    g_object_unref (doc);

    return page;
}

Glib::ustring
SnippetPrivate::get_pdf_path ()
{
    return Glib::build_filename (data_dir, PDF_FILENAME);
}
