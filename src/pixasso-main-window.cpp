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


using namespace Pixasso;

MainWindow::MainWindow ()
{
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Widget *widget;

    set_default_size (800, 400);
    set_border_width (10);
    set_title ("PiXasso");

    history = Glib::RefPtr<History> (new History ());

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso.ui"));
    } catch (const Glib::FileError &e) {
        std::cerr << e.what () << std::endl;
        exit (EXIT_FAILURE);
    }

    refBuilder->get_widget ("main-grid", widget);

    snippet_editor = new SnippetEditor ();
    ((Gtk::Grid *) widget)->attach (*snippet_editor, 0, 0, 1, 1);
    snippet_editor->set_size_request (300, -1);

    preview = new Preview ();
    ((Gtk::Grid *) widget)->attach (*preview, 1, 0, 1, 1);
    preview->set_hexpand (true);

    history_view = new HistoryView (history);
    ((Gtk::Grid *) widget)->attach (*history_view, 2, 0, 1, 2);

    widget->show_all ();
    add (*widget);

    history_view->get_treeview ()->signal_row_activated ().connect (sigc::mem_fun (*this,
                                                                                   &MainWindow::on_history_row_activated));

    refBuilder->get_widget ("apply-button", widget);
    ((Gtk::Button *) widget)->
        signal_clicked ().connect (sigc::mem_fun (*this,
                                                  &MainWindow::on_apply_button_clicked));

    refBuilder->get_widget ("clear-button", widget);
    ((Gtk::Button *) widget)->
        signal_clicked ().connect (sigc::mem_fun (*this,
                                                  &MainWindow::on_clear_button_clicked));

    refBuilder->get_widget ("history-button", widget);
    ((Gtk::ToggleButton *) widget)->set_active (true);
    ((Gtk::ToggleButton *) widget)->
        signal_clicked ().connect (sigc::mem_fun (*this, &MainWindow::on_history_button_clicked));
}

MainWindow::~MainWindow ()
{
    delete snippet_editor;
    delete history_view;
    delete preview;
}

void
MainWindow::on_history_row_activated (const Gtk::TreeModel::Path& path,
                                             Gtk::TreeViewColumn* /* column */)
{
    Glib::RefPtr<Snippet> snippet;
    (*history_view->get_treeview ()->get_model ()->get_iter (path)).get_value (0, snippet);
    preview->set_snippet (snippet);
    snippet_editor->fill_with_snippet (snippet);
}

void
MainWindow::on_apply_button_clicked ()
{
    try {
        Glib::RefPtr<Snippet> snippet;
        snippet = snippet_editor->create_snippet ();
        history->prepend_snippet (snippet);
        history_view->get_treeview ()->scroll_to_row (Gtk::TreePath ("0"));
        history_view->get_treeview ()->get_selection ()->unselect_all ();
        preview->set_snippet (snippet);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what () << std::endl;
    }
}

void
MainWindow::on_clear_button_clicked ()
{
    snippet_editor->set_default ();
    preview->clear ();
}

void
MainWindow::on_history_button_clicked ()
{
    if (history_view->get_visible ())
        history_view->hide ();
    else
        history_view->show ();
}
