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

#include "pixasso-main-window.h"

#include "pixasso-history-view.h"
#include "pixasso-preview.h"
#include "pixasso-snippet.h"
#include "pixasso-snippet-editor.h"

#include <gtkmm.h>
#include <iostream>


PixassoMainWindow::PixassoMainWindow ()
{
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Widget *widget;

    set_default_size (800, 400);
    set_border_width (10);
    //set_title ("PiXasso");

    history = Glib::RefPtr<PixassoHistory> (new PixassoHistory ());

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso.ui"));
    } catch (const Glib::FileError &e) {
        std::cerr << e.what () << std::endl;
        exit (EXIT_FAILURE);
    }

    refBuilder->get_widget ("grid1", widget);

    snippet_editor = new PixassoSnippetEditor ();
    ((Gtk::Grid *) widget)->attach (*snippet_editor, 0, 0, 1, 1);

    preview = new PixassoPreview ();
    ((Gtk::Grid *) widget)->attach (*preview, 1, 0, 1, 1);

    history_view = new PixassoHistoryView (history);
    ((Gtk::Grid *) widget)->attach (*history_view, 2, 0, 1, 2);

    widget->show_all ();
    add (*widget);

    history_view->get_treeview ()->signal_row_activated ().connect (sigc::mem_fun (*this,
                                                                                   &PixassoMainWindow::on_history_row_activated));

    refBuilder->get_widget ("apply-button", widget);
    widget->set_hexpand (true);
    ((Gtk::Button *) widget)->
        signal_clicked ().connect (sigc::mem_fun (*this,
                                                  &PixassoMainWindow::on_apply_button_clicked));

    refBuilder->get_widget ("clear-button", widget);
    widget->set_hexpand (true);
    ((Gtk::Button *) widget)->
        signal_clicked ().connect (sigc::mem_fun (*this,
                                                  &PixassoMainWindow::on_clear_button_clicked));

    refBuilder->get_widget ("history-button", history_button);
    history_button->signal_clicked ().connect (sigc::mem_fun (*this,
                                                              &PixassoMainWindow::on_history_button_clicked));
}

PixassoMainWindow::~PixassoMainWindow ()
{
    delete snippet_editor;
    delete history_view;
    delete preview;
}

void
PixassoMainWindow::on_history_row_activated (const Gtk::TreeModel::Path& path,
                                             Gtk::TreeViewColumn* /* column */)
{
    Glib::RefPtr<PixassoSnippet> snippet;
    (*history_view->get_treeview ()->get_model ()->get_iter (path)).get_value (0, snippet);
    preview->set_snippet (snippet);
    snippet_editor->fill_with_snippet (snippet);

    g_debug ("Application Activated snippet: %s", snippet->get_latex_body ().c_str ());
}

void
PixassoMainWindow::on_apply_button_clicked ()
{
    try {
        Glib::RefPtr<PixassoSnippet> snippet;
        snippet = snippet_editor->create_snippet ();
        history->prepend_snippet (snippet);
        preview->set_snippet (snippet);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what () << std::endl;
    }
}

void
PixassoMainWindow::on_clear_button_clicked ()
{
    snippet_editor->set_default ();
    preview->clear ();
}

void
PixassoMainWindow::on_history_button_clicked ()
{
    if (history_view->get_visible ())
        history_view->hide ();
    else
        history_view->show ();
}
