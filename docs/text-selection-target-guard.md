# Text selection target validation

The text selection responder receives the requested target through a pointer.
It now rejects a null target before inspecting the atom, matching its existing
validation of the selection and output pointers. This preserves normal X11
selection conversion while preventing malformed internal callers from causing
a null dereference.

The same responder uses explicit Xt `Cardinal` and wire-length conversions
after its existing upper-bound checks, keeping strict 64-bit builds from
silently narrowing allocation sizes.

The contract is covered by
`tests/automated/check-text-selection-target-guard.py`.
