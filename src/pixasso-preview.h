/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef PIXASSO_PREVIEW_H
#define PIXASSO_PREVIEW_H

#include "pixasso-snippet.h"

#include <gtkmm.h>


class PixassoPreview : public Gtk::Grid {
public:
    PixassoPreview ();
    PixassoPreview (PixassoSnippet *);
    ~PixassoPreview ();
    void set_snippet (PixassoSnippet *);
    void clear ();
    void set_zoom_factor (double);
    void set_zoom_in ();
    void set_zoom_out ();
    void set_zoom_100 ();
    double get_zoom_factor ();

    //signal accessor:
    //typedef sigc::signal<void, double> type_signal_zoom_factor_changed;
    //type_signal_zoom_factor_changed signal_zoom_factor_changed ();

private:
    class Area;
    Area *area;

    Gtk::Label *zoom_label;
    void setup_preview ();
    bool on_event_cb (GdkEvent *);

protected:
    //type_signal_zoom_factor_changed m_signal_zoom_factor_changed;
};

#endif

