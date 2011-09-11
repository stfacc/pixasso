bin_PROGRAMS = pixasso

INCLUDES = \
	-DDATADIR=\""$(datadir)"\"

pixasso_SOURCES = \
	main.cpp			\
	pixasso-app.cpp			\
	pixasso-app.h			\
	pixasso-history.cpp		\
	pixasso-history.h		\
	pixasso-history-view.cpp	\
	pixasso-history-view.h		\
	pixasso-misc-widgets.cpp	\
	pixasso-misc-widgets.h		\
	pixasso-preview.cpp		\
	pixasso-preview.h		\
	pixasso-snippet.cpp		\
	pixasso-snippet.h		\
	pixasso-snippet-editor.cpp	\
	pixasso-snippet-editor.h

pixasso_CPPFLAGS = \
	$(GTK_CFLAGS)		\
	$(POPPLER_GLIB_CFLAGS)

pixasso_LDFLAGS = \
	$(GTK_LIBS)		\
	$(POPPLER_GLIB_LIBS)

