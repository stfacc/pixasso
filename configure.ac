AC_INIT([PiXasso], [0.3], [stefano.facchini@gmail.com])
AM_INIT_AUTOMAKE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile data/Makefile src/Makefile)
PKG_CHECK_MODULES(GTK, gtkmm-3.0)
PKG_CHECK_MODULES(POPPLER_GLIB, poppler-glib)
AC_OUTPUT
