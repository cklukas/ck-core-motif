---
nav_exclude: true
---
# Optional C++ support-library proposal (WP-08)

This is a design boundary, not a replacement object model and not a required
`libXm` dependency. Existing C callers and the Xt/Motif ABI remain authoritative.
The proposal addresses the most repetitive C++ consumer work while keeping raw
widgets and resource names visible at the boundary.

## Proposed small surface

```cpp
namespace motif {
class string;                 // owned XmString; copy duplicates, move transfers
class args;                   // ordered XtArgVal storage; names remain Xm resource names
class widget_ref;             // non-owning Widget view; never destroys the widget
class callback_connection;    // RAII XtAddCallback/XtRemoveCallback registration
}
```

`motif::string` owns exactly one `XmString` and frees it with `XmStringFree`.
It has explicit UTF-8/localized construction, copy and move operations; no
implicit conversion to a mutable `char *` is provided. `motif::args` owns its
argument storage until the Xt call returns and forbids mutation after passing
the array to Xt. `widget_ref` is intentionally non-owning and can be empty.

`callback_connection` stores the callback identity and borrowed closure exactly
as Xt does. Destruction unregisters on the owning display thread. A callback
trampoline catches every C++ exception, records it in an application-owned
error sink and returns across the C ABI; exceptions never cross Xt. The
connection does not keep an arbitrary Widget alive, and widget destruction
invalidates a connection before its owner releases the wrapper.

## Interoperation and ownership rules

- Raw `Widget` values can be constructed into `widget_ref` and extracted for
  existing C calls; no private Xm records or global wrapper registry are used.
- Wrappers are movable, but only `string` owns toolkit memory. Copying a
  `widget_ref` or `args` view does not create widget ownership.
- All widget, display, Xt callback and wrapper destruction operations run on
  the owning Xt application thread.
- The support layer is an exception barrier and value-ownership aid, not a
  thread-safe widget facade or an asynchronous dispatch mechanism.
- The prototype remains header-only and consumer-side until an installed-prefix
  lifecycle test proves the callback/destruction contract.

## Consumer examples to validate

1. Build a localized `motif::string`, pass it to a normal Label resource and
   verify copy/move/destruction with an installed prefix.
2. Build ordered `motif::args` for a C-created widget, then reuse the same raw
   widget through `widget_ref` without private-record access.
3. Register a callback with a borrowed closure, destroy the widget first and
   release the `callback_connection` afterward; verify no callback or use-after-
   free occurs.
4. Throw from a callback and verify the exception sink receives it while Xt
   continues unwinding normally.

## Decision

WP-08 design is complete enough to prototype. The decision is **GO for a
consumer-only prototype** and **NO-GO for an installed WP-09 release yet**:
the lifecycle/exception-barrier fixture and a fresh C++17 installed-prefix
consumer must pass before any public support-library package is added. This
keeps the modernization additive and leaves the stable C ABI untouched.
