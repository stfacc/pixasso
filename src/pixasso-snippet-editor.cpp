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


PixassoSnippetEditor::PixassoSnippetEditor ()
{
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Widget *widget;

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso-snippet-editor.ui"));
    } catch (Glib::FileError &e) {
        g_error (e.what ().c_str ());
    }

    refBuilder->get_widget ("latex-textview", textView);
    refBuilder->get_widget ("font-entry", fontEntry);
    
    refBuilder->get_widget ("scrolledwindow", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);

    attach (*widget, 0, 0, 1, 1);
    
    refBuilder->get_widget ("textview-toolbar", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);

    attach (*widget, 0, 1, 1, 1);
    
    styleCombo = new PixassoLatexStyleCombo ();
    styleCombo->show ();
    refBuilder->get_widget ("toolitem-style-combo", widget);
    ((Gtk::Container *) widget)->add (*styleCombo);
}

PixassoSnippet *
PixassoSnippetEditor::create_snippet ()
{
    PixassoSnippet *snippet;
    snippet = new PixassoSnippet ("default",
				  textView->get_buffer ()->get_text (),
				  styleCombo->get_active_style ());

    return snippet;
}

void
PixassoSnippetEditor::fill_with_snippet (PixassoSnippet *snippet)
{
    textView->get_buffer ()->set_text (snippet->get_latex_body ());
}
