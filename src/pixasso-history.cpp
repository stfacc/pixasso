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

#include <gtkmm/treemodel.h>
#include <glibmm/fileutils.h>
#include <iostream>


using namespace Pixasso;

History::History ()
{
    g_debug ("Creating history list");
    set_column_types (m_Columns);

    populate ();
}

History::~History ()
{
}

void
History::prepend_snippet (Glib::RefPtr<Snippet> &snippet)
{
    g_debug ("Creating history element %s: %s --- %fx%f",
             snippet->get_data_dir ().c_str (),
             snippet->get_latex_body ().c_str (),
             snippet->get_width (),
             snippet->get_height ());

    Gtk::TreeModel::Row row = *(prepend ());
    row[m_Columns.m_Snippet] = snippet;
    row[m_Columns.m_LatexBody] = snippet->get_latex_body ();
}

void
History::populate ()
{
    Glib::ustring user_data_dir = Glib::get_user_data_dir ();
    Glib::ustring path = Glib::build_filename (user_data_dir, PACKAGE);
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
