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

#ifndef PIXASSO_HISTORY_H
#define PIXASSO_HISTORY_H

#include "pixasso-snippet.h"

#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>


class PixassoHistory : public Gtk::ListStore
{
public:
    enum {
        SNIPPET_C,
        N_COLUMNS
    };

    PixassoHistory ();
    ~PixassoHistory ();
    void prepend_snippet (Glib::RefPtr<PixassoSnippet> &snippet);

protected:
    void populate ();

    class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns()
        { add (m_Snippet); }

        Gtk::TreeModelColumn< Glib::RefPtr<PixassoSnippet> > m_Snippet;
    };

    ModelColumns m_Columns;
};

#endif
