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

#include "pixasso-history.h"

#include "pixasso-snippet.h"

#include <glib/gi18n.h>
#include <gtkmm/treemodel.h>
#include <glibmm/fileutils.h>
#include <glibmm/datetime.h>
#include <giomm/file.h>
#include <iostream>


using namespace Pixasso;

History::History ()
{
    set_column_types (m_Columns);
    set_sort_column (m_Columns.m_CreationTime, Gtk::SORT_DESCENDING);
    set_sort_func (m_Columns.m_CreationTime, sigc::mem_fun (*this, &History::compare_time));

    populate ();
}

int
History::compare_time (const Gtk::TreeModel::iterator & a,
                       const Gtk::TreeModel::iterator & b)
{
    Gtk::TreeModel::Row row_a = *a;
    Gtk::TreeModel::Row row_b = *b;

    if (row_a[m_Columns.m_CreationTime] > row_b[m_Columns.m_CreationTime])
        return 1;
    else if (row_a[m_Columns.m_CreationTime] < row_b[m_Columns.m_CreationTime])
        return -1;
    else
        return 0; // FAPP this is impossible!
}

History::~History ()
{
}

void
History::prepend_snippet (Glib::RefPtr<Snippet> &snippet)
{
    Gtk::TreeModel::Row row = *(prepend ());
    row[m_Columns.m_Snippet] = snippet;
    row[m_Columns.m_LatexBody] = snippet->get_latex_body ();
    row[m_Columns.m_CreationTime] = snippet->get_creation_time ();

    Glib::DateTime dt = Glib::DateTime::create_now_local (snippet->get_creation_time ());
    row[m_Columns.m_Tooltip] =
        Glib::Markup::escape_text (snippet->get_latex_body ()) + "\n"
        "Created: " + dt.format ("%c");
}

void
History::populate ()
{
    Glib::ustring user_data_dir = Glib::get_user_data_dir ();
    Glib::ustring path = Glib::build_filename (user_data_dir, PACKAGE);

    if (!Glib::file_test (path, Glib::FILE_TEST_EXISTS)) {
        Gio::File::create_for_path (path)->make_directory_with_parents ();
        return;
    }

    Glib::Dir dir (path);

    Glib::RefPtr<Snippet> snippet;
    Glib::ustring filename;

    Glib::DirIterator i;
    for (i = dir.begin (); i != dir.end (); i++) {
        try {
            filename = Glib::build_filename (user_data_dir, PACKAGE, *i);
            snippet = Glib::RefPtr<Snippet> (new Snippet (filename));
            prepend_snippet (snippet);
        } catch (Glib::Exception &e) {
            std::cerr << "Error creating history element: " << e.what () << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Error creating history element: " << e.what () << std::endl;
        }
    }
}
