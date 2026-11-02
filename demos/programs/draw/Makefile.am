MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/draw
datadir = ${prefix}/share/Xm/draw

bin_PROGRAMS = draw
draw_SOURCES = draw.c paint.c paint.h paint-icons.c paint-icons.h
EXTRA_DIST = XmdDraw.ad README draw.help draw.man
data_DATA = $(EXTRA_DIST) $(draw_SOURCES)
LDADD = ../../../lib/Xm/libXm.la -lXt -lX11 -lm
INCLUDES = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
