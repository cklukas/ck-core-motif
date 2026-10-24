# WP-09 optional C++ support-library release decision

WP-08 deliberately stopped at a consumer-side design and prototype boundary.
Its proposed `motif::string`, `motif::args`, `motif::widget_ref` and
`motif::callback_connection` types still require installed-prefix evidence for
callback exception barriers, widget-destruction ordering and mixed C/C++
ownership before they can become a separately versioned SDK component.

No such installed lifecycle fixture or package contract is present in the
current checkout.  Adding headers, build metadata or a library now would
create an unsupported ownership and exception ABI, not solve a measured
toolkit defect.

## Decision

**No-go for WP-09 release.**  Keep the proposal consumer-only and leave
`libXm`'s C ABI unchanged.  Do not install a partial C++ wrapper, hide a
global widget registry, or claim that a header-only sketch is an SDK.

Re-open this package only after the WP-08 fixture is implemented against a
fresh installed prefix and proves, at minimum:

- copied and moved `XmString` ownership with no leaks or double frees;
- ordered argument storage whose lifetime ends at the Xt call boundary;
- callback unregister/destruction ordering when the widget is destroyed first;
- an exception barrier that catches every exception before returning through
  the C callback ABI; and
- mixed C-created/C++-wrapped widget use without private-record access.

The future package must remain optional, separately versioned and independent
of a clean C `libXm` build.
