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

protected:
    virtual void render_vfunc (const Cairo::RefPtr< Cairo::Context > & cr,
                               Gtk::Widget& widget,
                               const Gdk::Rectangle& background_area,
                               const Gdk::Rectangle& cell_area,
                               Gtk::CellRendererState flags);

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

    history_model = history;
    m_TreeView.set_model (history_model);
    m_TreeView.set_tooltip_column (History::TOOLTIP_C);

    m_TreeView.get_selection ()->signal_changed ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_selection_changed));
    m_TreeView.get_selection ()->set_mode (Gtk::SELECTION_MULTIPLE);
    m_TreeView.set_rubber_banding (true);

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
    ((Gtk::ToolButton *) widget)->signal_clicked ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_clear_button_clicked));

    refBuilder->get_widget ("show-button", show_button);
    show_button->set_sensitive (false);
    show_button->signal_clicked ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_show_button_clicked));

    refBuilder->get_widget ("remove-button", remove_button);
    remove_button->set_sensitive (false);
    remove_button->signal_clicked ()
        .connect (sigc::mem_fun (*this, &HistoryView::on_remove_button_clicked));

    show_all_children ();
}

Gtk::TreeView *
HistoryView::get_treeview ()
{
    return &m_TreeView;
}

void
HistoryView::on_selection_changed ()
{
    gint selected_rows = m_TreeView.get_selection ()->count_selected_rows ();

    remove_button->set_sensitive (selected_rows > 0);
    show_button->set_sensitive (selected_rows == 1);
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
HistoryView::on_show_button_clicked ()
{
    // The button activating this callback is sensitive
    // only when the selection contains exactly one element 
    std::vector<Gtk::TreeModel::Path> row_paths = m_TreeView.get_selection ()->get_selected_rows ();
    m_TreeView.row_activated (row_paths[0], *(m_TreeView.get_column (0)));
}

void
HistoryView::on_remove_button_clicked ()
{
    Gtk::Window *parent = (Gtk::Window *) get_toplevel ();
    Gtk::MessageDialog dialog (*parent,
                               _("Delete selected snippets?"),
                               false,
                               Gtk::MESSAGE_WARNING,
                               Gtk::BUTTONS_OK_CANCEL);
    if (dialog.run () != Gtk::RESPONSE_OK)
        return;

    std::vector<Gtk::TreeModel::Path> row_paths = m_TreeView.get_selection ()->get_selected_rows ();
    std::vector<Gtk::TreeIter> row_iters;

    std::vector<Gtk::TreeModel::Path>::iterator i;
    for (i = row_paths.begin (); i != row_paths.end (); ++i)
        row_iters.push_back (m_TreeView.get_model ()->get_iter (*i));

    Glib::RefPtr<Snippet> snippet;
    std::vector<Gtk::TreeIter>::iterator j;
    Gtk::TreeIter iter;
    for (j = row_iters.begin (); j != row_iters.end (); ++j) {
        iter = *j;
        (*iter).get_value (History::SNIPPET_C, snippet);
        snippet->set_remove_data_on_delete (true);
        history_model->erase (iter);
    }
}

void
HistoryCellRenderer::render_vfunc (const Cairo::RefPtr< Cairo::Context > & cr,
                                   Gtk::Widget& widget,
                                   const Gdk::Rectangle& background_area,
                                   const Gdk::Rectangle& cell_area,
                                   Gtk::CellRendererState flags)
{
    Glib::RefPtr<Snippet> snippet = property_snippet ();

    // Draw a 1px line at the bottom of the cell
    Gdk::RGBA border_color = widget.get_style_context ()->get_border_color ();
    cr->set_source_rgb (border_color.get_red (),
                        border_color.get_green (),
                        border_color.get_blue ());
    cr->move_to (background_area.get_x (),
                 background_area.get_y () + background_area.get_height ());
    cr->line_to (background_area.get_x () + background_area.get_width (),
                 background_area.get_y () + background_area.get_height ());
    cr->set_line_width (1);
    cr->stroke ();

    // Draw the snippet. We add 2px margin at top and at bottom
    // between the line and the snippet
    int x = cell_area.get_x ();
    int y = cell_area.get_y () + 2;

    int snippet_area_width = cell_area.get_width ();
    int snippet_area_height = cell_area.get_height () - 4;

    double scale = MIN (MIN (snippet_area_width / snippet->get_width (),
                             snippet_area_height / snippet->get_height ()),
                        1);

    x += (snippet_area_width - snippet->get_width () * scale) / 2;
    y += (snippet_area_height - snippet->get_height () * scale) / 2;

    cr->translate (x, y);
    snippet->render (cr, scale);
}

void
HistoryCellRenderer::get_preferred_height_for_width_vfunc (Gtk::Widget& widget,
                                                           int /* width */,
                                                           int& minimum_height,
                                                           int& natural_height) const
{
    /* We want the cell to have a fixed aspect ratio (namely,
     * the golden ratio). To avoid vertical resizing when
     * the scrollbar appears or disappears, we use the width of
     * of the scrolled window to compute the height of the cell.
     */

    Gtk::ScrolledWindow *scrolled = (Gtk::ScrolledWindow *) widget.get_parent ();

    Gtk::Border border = scrolled->get_style_context ()->get_border ();
    Gtk::Border padding = scrolled->get_style_context ()->get_padding ();
    int width = scrolled->get_allocated_width () -
        border.get_left () - border.get_right () -
        padding.get_left () - padding.get_right ();

#define GOLDEN_RATION 1.618
    natural_height = minimum_height = width / GOLDEN_RATION;
}
