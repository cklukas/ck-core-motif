# WP-10 Form and hierarchy diagnostic corpus

The WP-10 fixture is an installed-library consumer, not a test of private
widget records.  `tests/automated/run-form-layout-corpus.py` builds
`check-form-layout-corpus.c` against a selected prefix and runs it under an
isolated Xvfb.  The fixture exercises a nested `XmForm`/`XmScrolledWindow`/
`XmRowColumn` hierarchy and performs:

- a twenty-step resize storm with changing shell dimensions;
- shell map/unmap and remap;
- dynamic child insertion/destruction and unmanage/manage operations;
- four push-button callback activations that mutate the hierarchy while the
  callback is running; and
- a repeated no-op resource update that must not trigger geometry negotiation
  or a visible Configure event; and
- final positive geometry and managed-state checks.

The output schema records `XtMakeGeometryRequest` and `XtQueryGeometry`
observations, selected Xlib redraw calls, Configure/Expose/Map/Unmap events,
and callback-triggered mutations.  The Xt/Xlib entry points are interposed by
the fixture only to make comparative measurements possible.  They are not a
complete accounting of private Form traversals, damage regions, server-side
painting, or time spent in descendants.

Each run writes a fresh `receipt.json`, compiler/link/run logs and
`form-layout-corpus.txt`.  The receipt includes the fixture hash, selected
prefix, output hash, metrics and elapsed times.  Results are diagnostic
evidence rather than a performance target; compare the same host, build,
display server and fixture before proposing a source correction.

The no-op control is a negative control for redundant work: after the hierarchy
settles, assigning the current `XmNmarginWidth` five times must produce zero
additional `XtMakeGeometryRequest` and Configure events.  A failure is a
regression in the fixture's expected control, not an invitation to change Form
without a matching source-level diagnosis.

## Current qualification scope

The corpus is qualified on the AArch64 `uconsole` Linux host with the current
installed prefix under Xvfb.  It establishes deterministic activity and
reentrancy coverage.  It does not yet justify a generic Form optimization:
the counters must be compared across an identified regression or hotspot, and
the next work package must preserve Xt geometry negotiation and final child
bounds.  x86_64, real multi-window-manager runs, cyclic/conflicting attachment
diagnostics and a focused Form correction remain open.
