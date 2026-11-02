MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/earth
datadir = ${prefix}/share/Xm/earth

bin_PROGRAMS = earth
earth_SOURCES = earth.c globe.h
earth_CPPFLAGS = $(INCLUDES) -DEARTH_MAP_FILE=\"$(datadir)/world-map.xpm\" -DEARTH_SOURCE_MAP_FILE=\"$(abs_srcdir)/world-map.xpm\"
EXTRA_DIST = terre.xbm earth.man README world-map.xpm MAP-LICENSE
data_DATA = $(EXTRA_DIST) $(earth_SOURCES)
LDADD = ../../../lib/Xm/libXm.la -lXt -lX11 -lXext -lm
INCLUDES = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS} 
