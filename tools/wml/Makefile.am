MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README wmllex.l motif.wml Uil.y docs/wml-build.md

AM_YFLAGS = -d
#AM_LFLAGS = -l

WMLTARGETS = UilConst.h	UilKeyTab.h	 \
	UilSymArTa.h	UilSymArTy.h	UilSymCSet.h	UilSymCtl.h \
	UilSymEnum.h	UilSymGen.h	UilSymNam.h	UilSymRArg.h \
	UilSymReas.h	UilTokName.h	UilUrmClas.h	UilSymChCl.h \
	UilSymChTa.h

TABLE = motif.wml
WMDTABLE = motif.wmd
WMLTOOLS = wml wmluiltok
REPORT = wml.report

wmldir = $(includedir)/uil

wml_HEADERS = UilDBDef.h

noinst_HEADERS = wml.h wmlparse.h $(WMLTARGETS)

noinst_PROGRAMS = wml wmluiltok wmldbcreate
wmluiltok_SOURCES = wmluiltok.c
wmluiltok_LDADD = @LEXLIB@

wml_SOURCES = wml.c
wml_LDADD = -L. -lwml
wml_DEPENDENCIES = libwml.a

wmldbcreate_SOURCES = wmldbcreate.c
wmldbcreate_LDADD = ../../lib/Xm/libXm.la -lXt -lX11

INCLUDES = -I../../lib -I$(srcdir)/../../lib -I$(srcdir)/../../include ${X_CFLAGS} 

noinst_LIBRARIES = libwml.a
libwml_a_SOURCES = wmlparse.y wml.c wmloutkey.c wmlouth.c wmloutmm.c wmloutp1.c wmlresolve.c wmlsynbld.c wmlutils.c

wmlparse.c: wmllex.c

wmllex.c: wmllex.l
	$(LEX) -o$@ $(srcdir)/wmllex.l

wmlsynbld.c: wmlparse.h

# Keep the lexer executable tied to the checked-in lexer source.  Relying on
# automake's implicit lex rule can reuse a stale generated file in a parallel
# or out-of-tree build, which used to hide the explicit main() entry point.
wmluiltok.c: wmluiltok.l
	$(LEX) -o$@ $(srcdir)/wmluiltok.l

# Uil.{c,h} are generated parser outputs, not source files.  Generate both
# through a stamp so parallel prerequisites cannot invoke yacc twice.
Uil.stamp: Uil.y
	$(RM) Uil.c Uil.h
	$(YACC) $(AM_YFLAGS) -o Uil.c $(srcdir)/Uil.y
	$(AM_V_at)touch $@

Uil.c Uil.h: Uil.stamp
	@:

# The object consumes generated headers; the C file is ordinary VPATH source.
wmldbcreate.o: $(WMLTARGETS) UilLexPars.h

# Generate the parser source/header pair once.  A stamp target keeps parallel
# builds from racing two recipes that both create the same symlinks.
UilLexPars.stamp: Uil.c Uil.h
	$(RM) UilLexPars.h UilLexPars.c
	$(LN_S) Uil.h UilLexPars.h
	$(LN_S) Uil.c UilLexPars.c
	$(AM_V_at)touch $@

UilLexPars.c UilLexPars.h: UilLexPars.stamp
	@:

$(WMLTARGETS): wml-uil.mm

wml-uil.mm: wmluiltok wml UilLexPars.c $(TABLE)
	$(RM) $(WMLTARGETS) wml.report wml-uil.mm
	./wmluiltok <$(srcdir)/Uil.y >tokens.dat
	$(RM) wml-uil.mm
	./wml $(srcdir)/$(TABLE)
	$(RM) tokens.dat

all: motif.wmd

motif.wmd: wmldbcreate
	./wmldbcreate -o motif.wmd

clean-local:
	$(RM) $(WMLTARGETS) $(WMDTABLE) $(REPORT) lex.yy.c libwml.a wml \
	*.mm *.sdml *.txt wmlparse.c wmlparse.h wmluiltok wmluiltok.c wmllex.c \
	tokens.dat DONE wmluiltok.c Uil.c Uil.h Uil.stamp wml.report \
	UilLexPars.[ch] UilLexPars.stamp ylwrap
 
