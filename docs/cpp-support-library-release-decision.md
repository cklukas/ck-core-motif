# WP-09 optional C++ support-library release decision

WP-08 now has a consumer-side header-only prototype and an installed-prefix
lifecycle fixture. Its proposed `motif::string`, `motif::args`,
`motif::widget_ref` and `motif::callback_connection` types pass the bounded
AArch64 fixture, including callback exception barriers, widget-destruction
ordering and mixed C/C++ ownership. The prototype remains deliberately outside
the installed SDK while its cross-compiler, sanitizer and ownership contract
are reviewed.

## Decision

**No-go for WP-09 release.** Keep the prototype consumer-only and leave
`libXm`'s C ABI unchanged. Do not install a partial C++ wrapper, hide a global
widget registry, or claim that one AArch64 fixture is an SDK qualification.

Re-open the package release decision only after the existing fixture is
repeated against a fresh installed prefix and proves, at minimum:

- copied and moved `XmString` ownership with no leaks or double frees;
- ordered argument storage whose lifetime ends at the Xt call boundary;
- callback unregister/destruction ordering when the widget is destroyed first;
- an exception barrier that catches every exception before returning through
  the C callback ABI; and
- mixed C-created/C++-wrapped widget use without private-record access.

The future package must remain optional, separately versioned and independent
of a clean C `libXm` build.
