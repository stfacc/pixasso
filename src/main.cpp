/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#include "pixasso-main-window.h"
#include "pixasso-utils.h"

#include <glib/gi18n.h>
#include <gtkmm.h>
#include <locale.h>


int main (int argc, char **argv)
{
    setlocale (LC_ALL, "");
    bindtextdomain (PACKAGE, LOCALE_DIR);
    textdomain (PACKAGE);

    Glib::ustring tmpdir;
    Gtk::Main kit (argc, argv);

    tmpdir = Pixasso::create_tmpdir ();
    chdir (tmpdir.c_str ());
    g_debug ("Working in: %s", tmpdir.c_str ());

    Pixasso::MainWindow window;
    kit.run (window);

    Pixasso::remove_dir (tmpdir);
}
