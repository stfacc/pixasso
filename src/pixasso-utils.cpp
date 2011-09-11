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

#include "pixasso-utils.h"

#include <giomm/file.h>
#include <glibmm/fileutils.h>
#include <glibmm/ustring.h>


Glib::ustring
Pixasso::create_tmpdir ()
{
    char *dir_cstr = g_build_filename (g_get_tmp_dir (), PACKAGE "XXXXXX", NULL);
    Glib::ustring dir;

    if (!mkdtemp (dir_cstr))
        throw std::runtime_error ("Cannot create temporary files");
    dir = Glib::ustring (dir_cstr);
    g_free (dir_cstr);

    return dir;
}

void
Pixasso::remove_dir (Glib::ustring dir_name)
{
    Glib::Dir dir (dir_name);

    Glib::DirIterator i;
    for (i = dir.begin (); i != dir.end (); i++)
        Gio::File::create_for_path (Glib::build_filename (dir_name, *i))->remove ();

    Gio::File::create_for_path (dir_name)->remove ();
}
