/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PIXASSO_SNIPPET_H
#define PIXASSO_SNIPPET_H

#include <glibmm/ustring.h>
#include <cairomm/context.h>

class PixassoSnippet {
public:
    typedef enum {
	DISPLAY = 0,
	INLINE,
	TEXT,
	N_LATEX_STYLE
    } LatexStyle;

    static const char *LatexStyleLabel [];
    
    PixassoSnippet (Glib::ustring p, Glib::ustring l, LatexStyle);
    PixassoSnippet (Glib::ustring d);
    ~PixassoSnippet ();
    void set_export_format ();
    Glib::ustring get_latex_body ();
    Glib::ustring get_preamble_name ();
    time_t get_creation_time ();
    Glib::ustring get_data_dir ();

    double get_width ();
    double get_height ();
    void render (Cairo::RefPtr<Cairo::Context>, double);
private:
    class Private;
    Private *priv;
};

#endif
