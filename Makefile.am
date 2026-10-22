datadir = ${prefix}/share/Xm/doc

#data_DATA = BUGREPORT COPYRIGHT.MOTIF COPYING \
#            README RELEASE RELNOTES

EXTRA_DIST = tests/accessibility/provider.c tests/accessibility/observe.py tests/accessibility/README.md \
             BUGREPORT \
             COPYING \
             README RELEASE RELNOTES \
             INSTALL.configure Makefile.imake-pure

MAINTAINERCLEANFILES = Makefile.in \
             configure libtool \
             config.guess \
             config.sub \
             config.log \
             ltconfig ltmain.sh \
             aclocal.m4 \
             config.h.in \
             install-sh \
             missing mkinstalldirs \
             compile

SUBDIRS	= bindings bitmaps \
             config \
             localized \
             lib \
             include \
             tools \
             clients \
             doc \
             demos
AUTOMAKE_OPTIONS = 1.4
ACLOCAL_AMFLAGS = -I .

# Focused fork manual; historical reference sources remain in doc/.
EXTRA_DIST += README.md ABI.md ckdocs.yml scripts/build-docs.py \
              tests/automated/check-docs-provenance.py \
              tests/automated/check-native-features.py \
              tests/Toolkit/ABI/cohort.c tests/Toolkit/Im/preedit_ranges.c \
              tests/Toolkit/Unicode/layout.c tests/Toolkit/Unicode/line_context.c \
              tests/Toolkit/Unicode/text_unicode.c \
              tests/disclosure/widget.c tests/disclosure/observe.py \
              docs/index.md docs/history.md docs/status.md docs/build.md \
              docs/abi.md docs/unicode.md docs/accessibility.md \
              docs/components.md docs/disclosure.md docs/documentation.md \
              .gitattributes .ckgit/build-commit .ckgit/ci.yml .github/workflows/pages.yml
