/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PIXASSO_MISC_WIDGETS_H
#define PIXASSO_MISC_WIDGETS_H

#include <gtkmm/comboboxtext.h>

#include "pixasso-snippet.h"

class PixassoLatexStyleCombo : public Gtk::ComboBoxText {
public:
    PixassoLatexStyleCombo ();
    PixassoSnippet::LatexStyle get_active_style ();
};

#endif
