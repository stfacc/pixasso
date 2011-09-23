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

#ifndef PIXASSO_SNIPPET_EXPORTER_H
#define PIXASSO_SNIPPET_EXPORTER_H

#include <glibmm.h>

namespace Pixasso
{

// The ifdef is here to silence compiler warnings
#ifndef PIXASSO_SNIPPET_H
    class Snippet;
#endif
    class SnippetPrivate;

    class SnippetExporter
    {
    public:
        SnippetExporter () : snippet (NULL)
        {}

        SnippetExporter (Glib::RefPtr<Snippet> x) : snippet (x)
        {}

        void set_snippet (Glib::RefPtr<Snippet> x)
        { snippet = x; }

        virtual Glib::ustring get_mime_type () const
        {};

        virtual gchar *get_data ()
        {};

    protected:
        SnippetPrivate *get_snippet_priv ();

        Glib::RefPtr<Snippet> snippet;
    };

    class SnippetExporterPlainText : public SnippetExporter
    {
    public:
        SnippetExporterPlainText () : SnippetExporter ()
        {}

        SnippetExporterPlainText (Glib::RefPtr<Snippet> snippet) : SnippetExporter (snippet)
        {}

        virtual Glib::ustring get_mime_type () const
        { return "text/plain"; }

        virtual gchar *get_data ();

        static Glib::ustring get_id ()
        { return "Plain text"; }
    };

    class SnippetExporterEpsUri : public SnippetExporter
    {
    public:
        SnippetExporterEpsUri () : SnippetExporter ()
        {}

        SnippetExporterEpsUri (Glib::RefPtr<Snippet> snippet) : SnippetExporter (snippet)
        {}

        virtual Glib::ustring get_mime_type () const
        { return "text/uri-list"; }

        virtual gchar *get_data ();

        static Glib::ustring get_id ()
        { return "EPS"; }

    private:
        void generate ();
    };

    class SnippetExporterPdfUri : public SnippetExporter
    {
    public:
        SnippetExporterPdfUri () : SnippetExporter ()
        {}

        SnippetExporterPdfUri (Glib::RefPtr<Snippet> snippet) : SnippetExporter (snippet)
        {}

        virtual Glib::ustring get_mime_type () const
        { return "text/uri-list"; }

        virtual gchar *get_data ();

        static Glib::ustring get_id ()
        { return "PDF"; }
    };

    class SnippetExporterPngUri : public SnippetExporter
    {
    public:
        SnippetExporterPngUri () : SnippetExporter ()
        {}

        SnippetExporterPngUri (Glib::RefPtr<Snippet> snippet) : SnippetExporter (snippet)
        {}

        virtual Glib::ustring get_mime_type () const
        { return "text/uri-list"; }

        virtual gchar *get_data ();

        static Glib::ustring get_id ()
        { return "PNG"; }

    private:
        void generate ();
    };

    class SnippetExporterFactory
    {
    public:
        enum ExportFormat {
            // Plain text is always the first
            EXPORT_PLAIN_TEXT,

            // Keep the following in alphabetical order
            EXPORT_EPS_URI,
            EXPORT_PDF_URI,
            EXPORT_PNG_URI,

            N_EXPORT
        };

        static SnippetExporter *create (ExportFormat format);
        static SnippetExporter *create (ExportFormat format, Glib::RefPtr<Snippet> snippet);
        static SnippetExporter *create (Glib::ustring id);
        static SnippetExporter *create (Glib::ustring id, Glib::RefPtr<Snippet> snippet);
        static Glib::ustring get_id (ExportFormat format);
    };

} // End namespace Pixasso

#endif // PIXASSO_SNIPPET_EXPORTER_H
