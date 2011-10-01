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

#include "pixasso-history-view.h"

#include "pixasso-snippet.h"
#include "pixasso-history.h"

#include <glib/gi18n.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/treeview.h>
#include <iostream>

using namespace Pixasso;

class HistoryCellRenderer : public Gtk::CellRenderer
{
public:
    HistoryCellRenderer () :
        Glib::ObjectBase (typeid (HistoryCellRenderer)),
        prop_snippet (*this, "snippet")
    {}

    Glib::PropertyProxy< Glib::RefPtr<Snippet> > property_snippet ()
    { return prop_snippet.get_proxy (); }
    const Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Snippet> > property_snippet () const
    { return Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Snippet> > (this, "snippet"); }

private:
    Glib::Property< Glib::RefPtr<Snippet> > prop_snippet;

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

HistoryView::HistoryView (Glib::RefPtr<History> &history)
{
    Glib::RefPtr<Gtk::Builder> refBuilder;

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso-history-view.ui"));
    } catch (Glib::FileError &e) {
        std::cerr << e.what () << std::endl;
        exit (EXIT_FAILURE);
    }

    // Build the Treeview
    HistoryCellRenderer *cell = Gtk::manage (new HistoryCellRenderer ());
    Gtk::TreeView::Column *column = Gtk::manage (new Gtk::TreeView::Column (_("History"), *cell));
    column->add_attribute (*cell, "snippet", History::SNIPPET_C);
    column->set_sort_column (History::CREATION_TIME_C);

    m_TreeView.append_column (*column);

    m_TreeView.signal_row_activated ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_row_activated));

    history_model = history;
    m_TreeView.set_model (history_model);
    m_TreeView.set_tooltip_column (History::TOOLTIP_C);

    Gtk::Widget *widget;
    refBuilder->get_widget ("history-scrolled", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);
    ((Gtk::ScrolledWindow *) widget)->add (m_TreeView);
    attach (*widget, 0, 0, 1, 1);

    // Build the Toolbar
    Gtk::Toolbar *toolbar;
    refBuilder->get_widget ("history-toolbar", toolbar);
    toolbar->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);
    attach (*toolbar, 0, 1, 1, 1);

    refBuilder->get_widget ("clear-button", widget);
    ((Gtk::Button *) widget)->signal_clicked ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_clear_button_clicked));

    refBuilder->get_widget ("remove-button", widget);
    ((Gtk::Button *) widget)->signal_clicked ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_remove_button_clicked));

    show_all_children ();
}

Gtk::TreeView *
HistoryView::get_treeview ()
{
    return &m_TreeView;
}

void
HistoryView::on_clear_button_clicked ()
{
    Gtk::Window *parent = (Gtk::Window *) get_toplevel ();
    Gtk::MessageDialog dialog (*parent,
                               _("Delete all snippets?"),
                               false,
                               Gtk::MESSAGE_WARNING,
                               Gtk::BUTTONS_OK_CANCEL);
    if (dialog.run () != Gtk::RESPONSE_OK)
        return;

    Gtk::TreeIter iter = history_model->get_iter ("0");
    Glib::RefPtr<Snippet> snippet;
    while (iter) {
        (*iter).get_value (History::SNIPPET_C, snippet);
        snippet->set_remove_data_on_delete (true);
        iter = history_model->erase (iter);
    }
}

void
HistoryView::on_remove_button_clicked ()
{
    Gtk::Window *parent = (Gtk::Window *) get_toplevel ();
    Gtk::MessageDialog dialog (*parent,
                               _("Delete selected snippet?"),
                               false,
                               Gtk::MESSAGE_WARNING,
                               Gtk::BUTTONS_OK_CANCEL);
    if (dialog.run () != Gtk::RESPONSE_OK)
        return;

    Glib::RefPtr<Snippet> snippet;
    Gtk::TreeIter iter = m_TreeView.get_selection ()->get_selected ();
    (*iter).get_value (History::SNIPPET_C, snippet);

    snippet->set_remove_data_on_delete (true);
    history_model->erase (iter);
}

void
HistoryView::on_row_activated (const Gtk::TreeModel::Path& path,
                                      Gtk::TreeViewColumn* /* column */)
{
    Glib::RefPtr<Snippet> snippet;
    (*m_TreeView.get_model ()->get_iter (path)).get_value (History::SNIPPET_C, snippet);
    g_debug ("Activated snippet: %s", snippet->get_latex_body ().c_str ());
}

void
HistoryCellRenderer::render_vfunc (const Cairo::RefPtr< Cairo::Context > & cr,
                                   Gtk::Widget& widget,
                                   const Gdk::Rectangle& background_area,
                                   const Gdk::Rectangle& cell_area,
                                   Gtk::CellRendererState flags)
{
    Glib::RefPtr<Snippet> snippet = property_snippet ();
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
    Glib::RefPtr<Snippet> snippet = property_snippet ();
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
