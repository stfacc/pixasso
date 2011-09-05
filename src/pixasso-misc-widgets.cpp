/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <gtkmm/comboboxtext.h>

#include "pixasso-misc-widgets.h"
#include "pixasso-snippet.h"

PixassoLatexStyleCombo::PixassoLatexStyleCombo ()
{
    for (int i = 0; i < PixassoSnippet::N_LATEX_STYLE; i++)
        append (PixassoSnippet::LatexStyleLabel[i]);
    set_active (0);
}

PixassoSnippet::LatexStyle
PixassoLatexStyleCombo::get_active_style ()
{
    return (PixassoSnippet::LatexStyle) get_active_row_number ();
}
