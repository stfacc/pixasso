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

#ifndef PIXASSO_SNIPPET_EDITOR_H
#define PIXASSO_SNIPPET_EDITOR_H

#include "pixasso-misc-widgets.h"
#include "pixasso-snippet.h"

#include <gtkmm.h>

namespace Pixasso
{

class SnippetEditor : public Gtk::Grid {
public:
    SnippetEditor ();
    void set_default ();
    Glib::RefPtr<Snippet> create_snippet ();
    void fill_with_snippet (Glib::RefPtr<Snippet> &);

private:
    Gtk::TextView *textView;
    Gtk::Entry *fontEntry;
    Gtk::ColorButton *colorButton;
    MathModeCombo *mathModeCombo;
};

} /* end namespace Pixasso */

#endif
