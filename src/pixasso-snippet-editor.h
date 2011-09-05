/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PIXASSO_SNIPPET_EDITOR_H
#define PIXASSO_SNIPPET_EDITOR_H

#include "pixasso-misc-widgets.h"
#include "pixasso-snippet.h"

#include <gtkmm.h>


class PixassoSnippetEditor : public Gtk::Grid {
public:
    PixassoSnippetEditor ();

    PixassoSnippet *get_snippet ();

private:    
    Gtk::TextView *textView;
    Gtk::Entry *fontEntry;
    PixassoLatexStyleCombo *styleCombo;
};

#endif
