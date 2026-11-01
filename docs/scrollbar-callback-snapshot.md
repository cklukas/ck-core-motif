# ScrollBar callback-list ownership

ScrollBar passes its compiled callback resource directly to
`XtCallCallbackList`. Xt owns this internal representation and retains the
executing list while callbacks add or remove entries.

Treating that storage as a public, null-terminated `XtCallbackRec` array and
copying it loses Xt's internal header. Multiple callbacks then receive corrupted
closures. A Scale with its value-conversion callback and an accessibility
observer exposed this as a crash on mouse release. No manual snapshot or
allocation is needed for native Xt callback resources.

The source guard is `tests/automated/check-scrollbar-callback-snapshot.py`.
The native `run-modern-scale.py --prefix PREFIX --out OUTPUT` fixture registers
an additional observer on Scale's child ScrollBar and verifies its closure. It
also removes and adds callbacks during dispatch and verifies both the current
and next delivery. Actual gallery press/release captures confirm accessibility
and the application feedback callback complete together.
