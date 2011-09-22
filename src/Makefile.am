bin_PROGRAMS = pixasso

INCLUDES = \
	-DLOCALE_DIR=\""$(localedir)"\"			\
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\"	\
	-DDATADIR=\""$(datadir)"\"

pixasso_SOURCES = \
	main.cpp			\
	pixasso-history.cpp		\
	pixasso-history.h		\
	pixasso-history-view.cpp	\
	pixasso-history-view.h		\
	pixasso-main-window.cpp		\
	pixasso-main-window.h		\
	pixasso-misc-widgets.cpp	\
	pixasso-misc-widgets.h		\
	pixasso-preview.cpp		\
	pixasso-preview.h		\
	pixasso-snippet.cpp		\
	pixasso-snippet.h		\
	pixasso-snippet-editor.cpp	\
	pixasso-snippet-editor.h	\
	pixasso-snippet-exporter.cpp	\
	pixasso-snippet-exporter.h	\
	pixasso-snippet-private.cpp	\
	pixasso-snippet-private.h	\
	pixasso-utils.cpp		\
	pixasso-utils.h

pixasso_CPPFLAGS = $(PIXASSO_CFLAGS)

pixasso_LDFLAGS = $(PIXASSO_LIBS)

LDADD = $(LIBINTL)
