/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <config.h>

#include "pixasso-snippet-editor.h"

#include "pixasso-misc-widgets.h"
#include "pixasso-snippet.h"

#include <gtkmm.h>


PixassoSnippetEditor::PixassoSnippetEditor ()
{
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Widget *widget;

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso-snippet-editor.ui"));
    } catch (Glib::FileError &e) {
        g_error (e.what ().c_str ());
    }

    refBuilder->get_widget ("latex-textview", textView);
    refBuilder->get_widget ("font-entry", fontEntry);
    
    refBuilder->get_widget ("scrolledwindow", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);

    attach (*widget, 0, 0, 1, 1);
    
    refBuilder->get_widget ("textview-toolbar", widget);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);

    attach (*widget, 0, 1, 1, 1);
    
    styleCombo = new PixassoLatexStyleCombo ();
    styleCombo->show ();
    refBuilder->get_widget ("toolitem-style-combo", widget);
    ((Gtk::Container *) widget)->add (*styleCombo);
}

PixassoSnippet *
PixassoSnippetEditor::get_snippet ()
{
    PixassoSnippet *snippet;
    snippet = new PixassoSnippet ("default",
				  textView->get_buffer ()->get_text (),
				  styleCombo->get_active_style ());

    return snippet;
}
