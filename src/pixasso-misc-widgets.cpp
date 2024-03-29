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

using namespace Pixasso;

MathModeCombo::MathModeCombo ()
{
    Snippet::MathModeMap::iterator i;
    for (i = Snippet::math_mode_map.begin ();
         i != Snippet::math_mode_map.end ();
         i++)
        append (i->first, i->second.display_id);
}

void
MathModeCombo::set_default ()
{
    get_active_id (Snippet::math_mode_map.begin ()->first);
}

Glib::ustring
MathModeCombo::get_active_math_mode ()
{
    return get_active_id ();
}
