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

#include "pixasso-history-view.h"

#include "pixasso-snippet.h"
#include "pixasso-history.h"

#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treeview.h>


PixassoHistoryView::PixassoHistoryView (Glib::RefPtr<PixassoHistory> &history)
{
    Gtk::CellRendererText *cell = Gtk::manage (new Gtk::CellRendererText ());
    Gtk::TreeView::Column *column = Gtk::manage (new Gtk::TreeView::Column ("Snippet", *cell));
    column->set_cell_data_func (*cell, sigc::mem_fun (*this, &PixassoHistoryView::cell_data_func)); 

    m_TreeView.append_column (*column);

    m_TreeView.signal_cursor_changed ().connect (sigc::mem_fun (*this,
                                                                &PixassoHistoryView::on_row_selected));

    m_TreeView.signal_row_activated ().connect (sigc::mem_fun (*this,
                                                                &PixassoHistoryView::on_row_activated));

    m_TreeView.set_model (history);

    set_shadow_type (Gtk::SHADOW_IN);
    add (m_TreeView);

    show_all_children ();
}

Gtk::TreeView *
PixassoHistoryView::get_treeview ()
{
    return &m_TreeView;
}

void
PixassoHistoryView::on_row_selected ()
{
    PixassoSnippet *snippet;
    (*m_TreeView.get_selection ()->get_selected ()).get_value (0, snippet);
    g_debug ("Selected snippet: %s", snippet->get_latex_body ().c_str ());
}

void
PixassoHistoryView::on_row_activated (const Gtk::TreeModel::Path& path,
                                      Gtk::TreeViewColumn* /* column */)
{
    PixassoSnippet *snippet;
    (*m_TreeView.get_model ()->get_iter (path)).get_value (0, snippet);
    g_debug ("Activated snippet: %s", snippet->get_latex_body ().c_str ());
}

void
PixassoHistoryView::cell_data_func (Gtk::CellRenderer *cell,
                                    const Gtk::TreeModel::iterator &iter)
{
    PixassoSnippet *snippet;
    (*iter).get_value (0, snippet);
    ((Gtk::CellRendererText *) cell)->property_text () = snippet->get_latex_body ();
}
