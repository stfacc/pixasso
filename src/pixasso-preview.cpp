/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "pixasso-preview.h"

#include "pixasso-snippet.h"

#include <cairomm/context.h>
#include <iomanip>


class PixassoPreview::Area
    : public Gtk::DrawingArea {
public:
    PixassoSnippet *snippet;
    double zoom_factor;

protected:
    virtual bool on_draw (const Cairo::RefPtr< Cairo::Context >& cr);
    virtual void get_preferred_width_vfunc (int& minimum_width, int& natural_width) const;
    virtual void get_preferred_height_vfunc (int& minimum_height, int& natural_height) const;
};


PixassoPreview::PixassoPreview ()
    : area (new Area ())
{
    setup_preview ();
    clear ();
}

PixassoPreview::PixassoPreview (PixassoSnippet *s)
    : area (new Area ())
{
    setup_preview ();
    set_snippet (s);
}

void
PixassoPreview::setup_preview ()
{
    Gtk::Widget *widget;

    widget = new Gtk::ScrolledWindow ();
    ((Gtk::ScrolledWindow *) widget)->add (*area);
    widget->signal_event ().connect (sigc::mem_fun (*this,
                                                &PixassoPreview::on_event_cb));

    widget->set_vexpand (true);
    widget->set_hexpand (true);
    ((Gtk::ScrolledWindow *) widget)->set_shadow_type (Gtk::SHADOW_IN);
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_BOTTOM);
    widget->show_all ();

    attach (*widget, 0, 0, 1, 1);

    widget = new Gtk::Toolbar ();
    widget->get_style_context ()->add_class ("inline-toolbar");
    widget->get_style_context ()->set_junction_sides (Gtk::JUNCTION_TOP);
    Gtk::ToolButton *tb;
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_IN));
    ((Gtk::Toolbar *) widget)->append (*tb, sigc::mem_fun (*this,
                                                           &PixassoPreview::set_zoom_in));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_OUT));
    ((Gtk::Toolbar *) widget)->append (*tb, sigc::mem_fun (*this,
                                                           &PixassoPreview::set_zoom_out));
    tb = new Gtk::ToolButton (Gtk::StockID (Gtk::Stock::ZOOM_100));
    ((Gtk::Toolbar *) widget)->append (*tb, sigc::mem_fun (*this,
                                                           &PixassoPreview::set_zoom_100));
    zoom_label = new Gtk::Label ();
    Gtk::ToolItem *ti = new Gtk::ToolItem ();
    ti->add (*zoom_label);
    ((Gtk::Toolbar *) widget)->append (*ti);
    
    widget->show_all ();

    attach (*widget, 0, 1, 1, 1);

    set_zoom_100 ();
}

PixassoPreview::~PixassoPreview ()
{
    delete area;
}
/*
PixassoPreview::type_signal_zoom_factor_changed
PixassoPreview::signal_zoom_factor_changed ()
{
    return m_signal_zoom_factor_changed;
}
*/
void
PixassoPreview::set_snippet (PixassoSnippet *s)
{
    area->snippet = s;
    area->queue_resize ();
}

void
PixassoPreview::clear ()
{
    set_snippet (NULL);
}

void
PixassoPreview::set_zoom_factor (double factor)
{
    area->zoom_factor = factor;
    zoom_label->set_text (Glib::ustring::format (std::fixed, std::setprecision (0), factor * 100) + "%");
    //m_signal_zoom_factor_changed.emit (factor);
    area->queue_resize ();
}

#define PIXASSO_PREVIEW_ZOOM_STEP_FACTOR 2
#define PIXASSO_PREVIEW_ZOOM_STEP_FACTOR_SCROLL 1.2

void
PixassoPreview::set_zoom_in ()
{
    set_zoom_factor (get_zoom_factor () * PIXASSO_PREVIEW_ZOOM_STEP_FACTOR);
}

void
PixassoPreview::set_zoom_out ()
{
    set_zoom_factor (get_zoom_factor () / PIXASSO_PREVIEW_ZOOM_STEP_FACTOR);
}


void
PixassoPreview::set_zoom_100 ()
{
    set_zoom_factor (1);
}

double
PixassoPreview::get_zoom_factor ()
{
    return area->zoom_factor;
}

bool
PixassoPreview::on_event_cb (GdkEvent *e)
{
    GdkEventScroll *event;

    if (e->type != GDK_SCROLL)
        return false;
    
    event = (GdkEventScroll *) e;
    
    switch (event->direction) {
    case GDK_SCROLL_UP:
        set_zoom_factor (get_zoom_factor () * PIXASSO_PREVIEW_ZOOM_STEP_FACTOR_SCROLL);
	break;
    case GDK_SCROLL_DOWN:
        set_zoom_factor (get_zoom_factor () / PIXASSO_PREVIEW_ZOOM_STEP_FACTOR_SCROLL);
        break;
    }
    g_debug ("ZOOM: %f", get_zoom_factor ());
    area->queue_resize ();
    
    return true;
}

void
PixassoPreview::Area::get_preferred_width_vfunc (int& minimum_width,
                                                 int& natural_width) const
{
    int w;
    if (snippet)
        w = ceil (snippet->get_width () * zoom_factor);
    else
        w = 0;
    minimum_width = natural_width = w;
}

void
PixassoPreview::Area::get_preferred_height_vfunc (int& minimum_height,
                                                  int& natural_height) const
{
    int h;
    if (snippet)
        h = ceil (snippet->get_height () * zoom_factor);
    else
        h = 0;
    minimum_height = natural_height = h;
}

bool
PixassoPreview::Area::on_draw (const Cairo::RefPtr< Cairo::Context >& cr)
{
    int min_width;
    int min_height;
    int nat_width;
    int nat_height;
    int x;
    int y;

    cr->set_source_rgb (1, 1, 1);
    cr->paint ();
    
    if (!snippet)
        return true;

    get_preferred_width (min_width, nat_width);
    get_preferred_height (min_height, nat_height);

    x = floor ((get_allocation ().get_width () - min_width) / 2);
    y = floor ((get_allocation ().get_height () - min_height) / 2);
    
    cr->translate (x, y);
    snippet->render (cr, zoom_factor);

    return true;
}
