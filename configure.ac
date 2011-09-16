AC_INIT([PiXasso], [0.3], [stefano.facchini@gmail.com])
AM_INIT_AUTOMAKE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CXX
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile data/Makefile src/Makefile po/Makefile.in)

# i18n stuff
IT_PROG_INTLTOOL([0.40])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

GETTEXT_PACKAGE=pixasso
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext Package])
AC_SUBST(GETTEXT_PACKAGE)

GTKMM_REQUIRED=3.1.90

PKG_CHECK_MODULES(PIXASSO, [
    gtkmm-3.0 >= $GTKMM_REQUIRED
    poppler-glib
])

AC_OUTPUT
