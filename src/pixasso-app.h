/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PIXASSO_APP_H
#define PIXASSO_APP_H

#include "pixasso-preview.h"
#include "pixasso-snippet.h"
#include "pixasso-snippet-editor.h"

#include <gtkmm.h>


class PixassoApp {
public:
    PixassoApp (int, char **);
    ~PixassoApp ();

private:
    char *tmpdir;

    Gtk::Window *mainWindow;
    PixassoSnippet *current_snippet;
    PixassoSnippetEditor *snippet_editor;
    PixassoPreview *preview;

protected:
    void on_apply_button_clicked ();
};

#endif
