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

#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>


namespace Pixasso
{

class HistoryView : public Gtk::Grid
{
public:
    HistoryView (Glib::RefPtr<History> &);
    Gtk::TreeView *get_treeview ();

protected:
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<History> history_model;

    void on_clear_button_clicked ();
    void on_remove_button_clicked ();
    void on_row_activated (const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
};

} /* end namespace Pixasso */

#endif
