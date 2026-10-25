# Deterministic display-scale override

`<Xm/Display.h>` now exposes an additive test/developer hook for simulating a
display scale without changing the caller's X resource database:

```c
XmSetDisplayScaleOverride(display, 2.0);
/* create or measure widgets at a deterministic 2x default scale */
XmClearDisplayScaleOverride(display);
```

`XmGetDisplayScaleOverride` reports whether an override is active.  The setter
accepts only finite, positive values; invalid values leave the current value
untouched.  Overrides are stored per `Display`, are process-local, and are
removed when cleared or when the Motif display object is destroyed.  They do
not call `XrmPutStringResource`, alter `Xft.dpi`, or rewrite an application's
resource database.

The override is consulted before the optional XLibre output-DPI adapter and
the existing global `Xft.dpi` fallback.  This gives a gallery or native test
fixture one deterministic scale transaction input while retaining the same
widget default conversion and saturation rules used by the real backends.
Explicit application pixel resources remain authoritative; only toolkit-owned
defaults that already participate in the DPI contract are affected.

This is the first WP-05.2 increment.  It does not claim live relayout of an
already-realized hierarchy or real multi-output qualification.  Those remain
separate work packages with stronger geometry and XLibre evidence requirements.

The installed consumer fixture is
`tests/automated/run-display-scale-override.py`.  It checks invalid values,
per-display retention, resource-database identity, default indicator scaling,
and reversible cleanup against the selected installed `libXm`.
