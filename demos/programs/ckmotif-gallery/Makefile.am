MAINTAINERCLEANFILES = Makefile.in
PYTHON = @PYTHON@

bin_PROGRAMS = ckmotif-gallery
AM_CFLAGS = -Wall -Wextra -Werror

ckmotif_gallery_SOURCES = gallery.c gallery_examples.c gallery_manifest.h gallery_sources.h
BUILT_SOURCES = gallery_manifest.h gallery_sources.h gallery_provenance.json
CLEANFILES = $(BUILT_SOURCES)
ckmotif_gallery_DEPENDENCIES = $(BUILT_SOURCES)

gallery_manifest.h gallery_sources.h gallery_provenance.json: $(srcdir)/../../../docs/modern-gallery-manifest.json \
        $(srcdir)/generate-gallery-assets.py $(srcdir)/gallery_examples.c
	$(AM_V_GEN)$(PYTHON) $(srcdir)/generate-gallery-assets.py \
	  $(srcdir)/../../../docs/modern-gallery-manifest.json \
	  $(srcdir)/gallery_examples.c $(builddir)

EXTRA_DIST = generate-gallery-assets.py gallery_examples.c README
LDADD = $(top_builddir)/lib/Xm/libXm.la -lXt -lX11
INCLUDES = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}

gallery_manifest.h gallery_sources.h: | $(srcdir)/gallery_examples.c

check-local: gallery_provenance.json $(top_srcdir)/docs/modern-gallery-components.md
	$(AM_V_at)$(PYTHON) $(top_srcdir)/tests/automated/check-gallery-source.py
	$(AM_V_at)$(PYTHON) $(top_srcdir)/tests/automated/check-gallery-provenance.py \
	  $(srcdir)/../../../docs/modern-gallery-manifest.json \
	  $(srcdir)/gallery_examples.c $(builddir)/gallery_provenance.json \
	  $(top_srcdir)/docs/modern-gallery-components.md
