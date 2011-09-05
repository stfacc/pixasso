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

#include <gtkmm/comboboxtext.h>

#include "pixasso-misc-widgets.h"
#include "pixasso-snippet.h"

PixassoLatexStyleCombo::PixassoLatexStyleCombo ()
{
    for (int i = 0; i < PixassoSnippet::N_LATEX_STYLE; i++)
        append (PixassoSnippet::LatexStyleLabel[i]);
    set_active (0);
}

PixassoSnippet::LatexStyle
PixassoLatexStyleCombo::get_active_style ()
{
    return (PixassoSnippet::LatexStyle) get_active_row_number ();
}
