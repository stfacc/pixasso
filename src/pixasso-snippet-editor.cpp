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

#include "pixasso-snippet-editor.h"

#include "pixasso-misc-widgets.h"
#include "pixasso-snippet.h"

#include <gtkmm.h>
#include <iostream>

#define DEFAULT_FONT_SIZE "14pt"
#define DEFAULT_COLOR "Black"

PixassoSnippetEditor::PixassoSnippetEditor ()
{
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Widget *widget;

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso-snippet-editor.ui"));
    } catch (Glib::FileError &e) {
        std::cerr << e.what () << std::endl;
        exit (EXIT_FAILURE);
    }

    refBuilder->get_widget ("latex-textview", textView);
    refBuilder->get_widget ("font-entry", fontEntry);
    refBuilder->get_widget ("color-button", colorButton);

    refBuilder->get_widget ("scrolledwindow", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);

    attach (*widget, 0, 0, 1, 1);

    refBuilder->get_widget ("textview-toolbar", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);

    attach (*widget, 0, 1, 1, 1);

    mathModeCombo = new PixassoMathModeCombo ();
    mathModeCombo->show ();
    refBuilder->get_widget ("toolitem-style-combo", widget);
    ((Gtk::Container *) widget)->add (*mathModeCombo);

    // We need to set this explicitly to override hexpand=true
    // of the ScrolledWindow child
    set_hexpand (false);

    set_default ();
}

void
PixassoSnippetEditor::set_default ()
{
    textView->get_buffer ()->set_text ("");
    fontEntry->set_text (DEFAULT_FONT_SIZE);
    colorButton->set_rgba (Gdk::RGBA (DEFAULT_COLOR));
    mathModeCombo->set_default ();
}

Glib::RefPtr<PixassoSnippet>
PixassoSnippetEditor::create_snippet ()
{
    return Glib::RefPtr<PixassoSnippet> (new PixassoSnippet ("default",
                                                             fontEntry->get_text (),
                                                             colorButton->get_rgba (),
                                                             mathModeCombo->get_active_math_mode (),
                                                             textView->get_buffer ()->get_text () ));
}

void
PixassoSnippetEditor::fill_with_snippet (Glib::RefPtr<PixassoSnippet> &snippet)
{
    fontEntry->set_text (snippet->get_font_size ());
    colorButton->set_rgba (snippet->get_color ());
    mathModeCombo->get_active_id (snippet->get_math_mode ());
    textView->get_buffer ()->set_text (snippet->get_latex_body ());
}
