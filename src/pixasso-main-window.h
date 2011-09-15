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

#ifndef PIXASSO_MAIN_WINDOW_H
#define PIXASSO_MAIN_WINDOW_H

#include "pixasso-history-view.h"
#include "pixasso-preview.h"
#include "pixasso-snippet.h"
#include "pixasso-snippet-editor.h"

#include <gtkmm/window.h>


class PixassoMainWindow : public Gtk::Window
{
public:
    PixassoMainWindow ();
    ~PixassoMainWindow ();

private:
    Glib::RefPtr<PixassoHistory> history;

    PixassoSnippetEditor *snippet_editor;
    PixassoPreview *preview;
    PixassoHistoryView *history_view;

protected:
    void on_history_row_activated (const Gtk::TreeModel::Path&,
                                   Gtk::TreeViewColumn*);
    void on_apply_button_clicked ();
    void on_clear_button_clicked ();
    void on_history_button_clicked ();
};

#endif
