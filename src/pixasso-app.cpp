/* -*- indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <config.h>

#include "pixasso-app.h"

#include "pixasso-preview.h"
#include "pixasso-snippet.h"
#include "pixasso-snippet-editor.h"

#include <gtkmm.h>


static void create_and_change_to_tmpdir (char **dir);
static void remove_dir (char *dir_name);


PixassoApp::PixassoApp (int argc, char **argv)
{
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Widget *widget;

    Gtk::Main kit (argc, argv);

    create_and_change_to_tmpdir (&tmpdir);

    try {
        refBuilder = Gtk::Builder::create_from_file (Glib::build_filename (DATADIR, PACKAGE,
                                                                           "pixasso.ui"));
    } catch (const Glib::FileError &e) {
        g_error (e.what ().c_str ());
    }

    snippet_editor = new PixassoSnippetEditor ();
    snippet_editor->show ();
    refBuilder->get_widget ("box2", widget);
    ((Gtk::Box *) widget)->pack_start (*snippet_editor);
    ((Gtk::Box *) widget)->reorder_child (*snippet_editor, 0);

    preview = new PixassoPreview ();
    preview->show ();
    refBuilder->get_widget ("box1", widget);
    ((Gtk::Box *) widget)->pack_start (*preview);

    refBuilder->get_widget ("apply_button", widget);
    ((Gtk::Button *) widget)->
        signal_clicked ().connect (sigc::mem_fun (*this,
                                                  &PixassoApp::on_apply_button_clicked));

    refBuilder->get_widget ("main_window", widget);
    kit.run (*(Gtk::Window *)widget);

    delete widget;
}

PixassoApp::~PixassoApp ()
{
    if (current_snippet)
        delete current_snippet;
    
    remove_dir (tmpdir);
    g_free (tmpdir);
}

void
PixassoApp::on_apply_button_clicked ()
{

    if (current_snippet)
        delete current_snippet;

    current_snippet = snippet_editor->get_snippet ();

    preview->set_snippet (current_snippet);
}

static void
create_and_change_to_tmpdir (char **dir)
{
    *dir = g_build_filename (g_get_tmp_dir (), PACKAGE "XXXXXX", NULL);

    if (!mkdtemp (*dir))
        g_error ("Cannot create temporary files");
  
    chdir (*dir);

    g_debug ("Working in: %s", *dir);
}

static void
remove_dir (gchar *dir_name)
{
    GFile *dir = g_file_new_for_path (dir_name);
    GFileEnumerator *file_enum = g_file_enumerate_children (dir,
                                                            G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                            G_FILE_QUERY_INFO_NONE,
                                                            NULL, NULL);
    GFileInfo *file_info;
    GFile *tmpfile;

    while (file_info = g_file_enumerator_next_file (file_enum, NULL, NULL)) {
        tmpfile = g_file_new_for_path (g_file_info_get_name (file_info));
        g_file_delete (tmpfile, NULL, NULL);
        g_object_unref (tmpfile);
    }

    g_file_delete (dir, NULL, NULL);
    g_object_unref (dir);
}
