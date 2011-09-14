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


class HistoryCellRenderer : public Gtk::CellRenderer
{
public:
    HistoryCellRenderer () :
        Glib::ObjectBase (typeid (HistoryCellRenderer)),
        prop_snippet (*this, "snippet")
    {}

    Glib::PropertyProxy< Glib::RefPtr<PixassoSnippet> > property_snippet ()
    { return prop_snippet.get_proxy (); }
    const Glib::PropertyProxy_ReadOnly< Glib::RefPtr<PixassoSnippet> > property_snippet () const
    { return Glib::PropertyProxy_ReadOnly< Glib::RefPtr<PixassoSnippet> > (this, "snippet"); }

private:
    Glib::Property< Glib::RefPtr<PixassoSnippet> > prop_snippet;

    static const int MINIMUM_CELL_HEIGHT = 50;

protected:
    virtual void render_vfunc (const Cairo::RefPtr< Cairo::Context > & cr,
                               Gtk::Widget& widget,
                               const Gdk::Rectangle& background_area,
                               const Gdk::Rectangle& cell_area,
                               Gtk::CellRendererState flags);
    virtual void get_preferred_width_vfunc (Gtk::Widget& widget,
                                            int& minimum_width,
                                            int& natural_width) const;
    virtual void get_preferred_height_for_width_vfunc (Gtk::Widget& widget,
                                                       int width,
                                                       int& minimum_height,
                                                       int& natural_height) const;
};

PixassoHistoryView::PixassoHistoryView (Glib::RefPtr<PixassoHistory> &history)
{
    set_policy (Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    set_size_request (150, -1);

    HistoryCellRenderer *cell = Gtk::manage (new HistoryCellRenderer ());
    Gtk::TreeView::Column *column = Gtk::manage (new Gtk::TreeView::Column ("History", *cell));
    column->add_attribute (*cell, "snippet", PixassoHistory::SNIPPET_C);

    m_TreeView.append_column (*column);

    m_TreeView.signal_cursor_changed ()
        .connect (sigc::mem_fun (*this, &PixassoHistoryView::on_row_selected));

    m_TreeView.signal_row_activated ()
        .connect (sigc::mem_fun (*this, &PixassoHistoryView::on_row_activated));

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
    Glib::RefPtr<PixassoSnippet> snippet;
    (*m_TreeView.get_selection ()->get_selected ()).get_value (PixassoHistory::SNIPPET_C, snippet);
    g_debug ("Selected snippet: %s", snippet->get_latex_body ().c_str ());
}

void
PixassoHistoryView::on_row_activated (const Gtk::TreeModel::Path& path,
                                      Gtk::TreeViewColumn* /* column */)
{
    Glib::RefPtr<PixassoSnippet> snippet;
    (*m_TreeView.get_model ()->get_iter (path)).get_value (PixassoHistory::SNIPPET_C, snippet);
    g_debug ("Activated snippet: %s", snippet->get_latex_body ().c_str ());
}

void
HistoryCellRenderer::render_vfunc (const Cairo::RefPtr< Cairo::Context > & cr,
                                   Gtk::Widget& widget,
                                   const Gdk::Rectangle& background_area,
                                   const Gdk::Rectangle& cell_area,
                                   Gtk::CellRendererState flags)
{
    Glib::RefPtr<PixassoSnippet> snippet = property_snippet ();
    int x = cell_area.get_x ();
    int y = cell_area.get_y ();
    double scale = cell_area.get_width () / snippet->get_width ();

    if (scale > 1) {
        x += (cell_area.get_width () - snippet->get_width ()) / 2;
        scale = 1;
    }
    y += (cell_area.get_height () - snippet->get_height () * scale) / 2;

    cr->translate (x, y);
    snippet->render (cr, scale);
}

void
HistoryCellRenderer::get_preferred_height_for_width_vfunc (Gtk::Widget& widget,
                                                           int width,
                                                           int& minimum_height,
                                                           int& natural_height) const
{
    Glib::RefPtr<PixassoSnippet> snippet = property_snippet ();
    double snippet_width = snippet->get_width ();
    double snippet_height = snippet->get_height ();
    natural_height = minimum_height = MAX (MIN (ceil (snippet_height), ceil (snippet_height * width / snippet_width)), MINIMUM_CELL_HEIGHT);
}

void
HistoryCellRenderer::get_preferred_width_vfunc (Gtk::Widget& widget,
                                                int& minimum_width,
                                                int& natural_width) const
{
    minimum_width = natural_width = widget.get_allocated_width ();
}
