# Copyright (C) 2019, Jian Lin
# License: GPL v3

plugindir = $(pkglibdir)/plugins

if BUILD_RULEWARN_PLUGIN
plugin_LTLIBRARIES = rulewarn.la
endif

rulewarn_la_SOURCES = \
	rulewarn.c

rulewarn_la_LDFLAGS = \
	-avoid-version -module \
	$(GTK_LIBS)

rulewarn_la_LIBADD = \
	$(GTK_LIBS)

rulewarn_la_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_builddir)/src/common \
	-I$(top_srcdir)/src/gtk \
	$(GLIB_CFLAGS) \
	$(GTK_CFLAGS)

.PHONY: test
