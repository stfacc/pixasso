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

#include "pixasso-misc-widgets.h"

#include "pixasso-snippet.h"

#include <gtkmm/comboboxtext.h>


PixassoMathModeCombo::PixassoMathModeCombo ()
{
    PixassoSnippet::MathModeMap::iterator i;
    for (i = PixassoSnippet::math_mode_map.begin ();
         i != PixassoSnippet::math_mode_map.end ();
         i++)
        append (i->first);
}

void
PixassoMathModeCombo::set_default ()
{
    set_active_text (PixassoSnippet::math_mode_map.begin ()->first);
}

Glib::ustring
PixassoMathModeCombo::get_active_math_mode ()
{
    // Should just be:
    //   return get_active_text ();
    // which does not work because of a bug in GTK 3.0

    Gtk::TreeRow row = *(get_active ());
    Glib::ustring s;
    row.get_value (0, s);
    return s;
}
