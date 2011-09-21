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

#include <cairomm/context.h>
#include <gdkmm/rgba.h>
#include <glibmm/convert.h>
#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
#include <glibmm/object.h>
#include <glibmm/spawn.h>
#include <glibmm/ustring.h>
#include <map>


namespace Pixasso
{

class SnippetExporter;
class SnippetExporterPlainText;
class SnippetExporterEpsUri;
class SnippetExporterPdfUri;
class SnippetExporterPngUri;

class Snippet : public Glib::Object
{
public:
    typedef enum {
        // Plain text is always the first
        EXPORT_PLAIN_TEXT,

        // Keep the following in alphabetical order
        EXPORT_EPS_URI,
        EXPORT_PDF_URI,
        EXPORT_PNG_URI,

        N_EXPORT
    } ExportFormat;

    typedef struct {
        Glib::ustring prefix;
        Glib::ustring suffix;
        Glib::ustring display_id;
    } MathMode;

    SnippetExporterPlainText *exporter_plain_text;
    SnippetExporterEpsUri *exporter_eps_uri;
    SnippetExporterPdfUri *exporter_pdf_uri;
    SnippetExporterPngUri *exporter_png_uri;

    typedef std::map<Glib::ustring, MathMode> MathModeMap;

    static MathModeMap math_mode_map;

    Snippet (Glib::ustring  /* preamble name */,
                    Glib::ustring  /* font size */,
                    Gdk::RGBA      /* color */,
                    Glib::ustring  /* math mode */,
                    Glib::ustring  /* latex body */);

    Snippet (Glib::ustring);
    ~Snippet ();

    Glib::ustring get_data_dir ();
    time_t get_creation_time ();

    Glib::ustring get_preamble_name ();
    Glib::ustring get_font_size ();
    Gdk::RGBA get_color ();
    Glib::ustring get_math_mode ();
    Glib::ustring get_latex_body ();
    Glib::ustring get_latex_full ();

    SnippetExporter *get_exporter (ExportFormat);

    void set_remove_data_on_delete (bool);

    double get_width ();
    double get_height ();
    bool is_generated ();
    void render (Cairo::RefPtr<Cairo::Context>, double);

private:
    void setup_exporters ();

    class Private;
    Private *priv;
};

class SnippetExporter
{
public:
    SnippetExporter (Snippet &x) : snippet (x)
    {}
    virtual Glib::ustring get_mime_type () const
    {};
    virtual gchar *get_data ()
    {};
    virtual bool is_generated ()
    {};

protected:
    Snippet &snippet;
};

} /* end namespace Pixasso */

#endif
