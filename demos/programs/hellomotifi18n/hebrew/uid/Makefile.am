MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = l_strings.uil
uiddir = ${prefix}/share/Xm/helloint/hebrew/uid
uid_DATA = l_strings.uid
CLEANFILES = l_strings.uid
l_strings.uid: $(srcdir)/l_strings.uil
	$(top_builddir)/clients/uil/uil $(srcdir)/l_strings.uil -I$(srcdir) -o $@
