---
nav_exclude: true
---
# WP-09 optional C++ support-library release decision

WP-08's consumer-side header and lifecycle fixture now qualify the installed
`ckmotif++` package. Its `motif::string`, `motif::args`, `motif::widget_ref`
and `motif::callback_connection` types pass a fresh AArch64 installed prefix
with GCC and Clang, including callback exception barriers,
widget-destruction ordering, mixed C/C++ ownership and sanitizer runs with
leak detection enabled.

## Decision

**Go for WP-09 release.** Install the header-only, separately versioned
`ckmotif++-8` package and its pkg-config metadata. Keep `libXm`'s C ABI
unchanged; do not hide a global widget registry or claim thread-safe widgets.

The accepted release evidence is a fresh installed prefix and proves, at
minimum:

- copied and moved `XmString` ownership with no leaks or double frees;
- ordered argument storage whose lifetime ends at the Xt call boundary;
- callback unregister/destruction ordering when the widget is destroyed first;
- an exception barrier that catches every exception before returning through
  the C callback ABI; and
- mixed C-created/C++-wrapped widget use without private-record access.

The package remains optional, separately versioned and independent of a clean C
`libXm` build. The native AArch64 result does not close the toolkit-wide
x86_64 qualification gate.

## Installation contract

Configure with `--enable-cxx-support` to install the header and
`ckmotif++-8.pc`. The default C build does not install or compile this optional
package, and `libXm` retains its unchanged C ABI. The installed-prefix
consumer fixture remains the release gate for callback, ownership and
destruction behavior.

The clean qualification build also covers the core C library: the scroll-frame
navigator capacity guard includes the standard integer-limit definitions it
uses, so the optional package does not depend on an incidental transitive
include from a particular compiler or system header set.
