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

#ifndef PIXASSO_HISTORY_VIEW_H
#define PIXASSO_HISTORY_VIEW_H

#include "pixasso-history.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

class PixassoHistoryView : public Gtk::ScrolledWindow
{
public:
    PixassoHistoryView (Glib::RefPtr<PixassoHistory> &);
    Gtk::TreeView *get_treeview ();

protected:
    Gtk::TreeView m_TreeView;
    void on_row_selected ();
    void on_row_activated (const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
    void cell_data_func (Gtk::CellRenderer*, const Gtk::TreeModel::iterator&);
};

#endif
