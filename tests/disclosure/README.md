# Native disclosure regression tests

`widget.c` creates real native windows. It checks preferred expanded/collapsed
geometry, retained content and descendant managed states, focus evacuation,
notification/no-notification behavior, keyboard activation and directional
navigation including right-to-left layout, native accessibility state/actions,
and destruction from the state callback. It does not establish application
visual acceptance or installed-library ABI acceptance when statically compiled
against an existing library.

For an isolated provisional test against an installed library, compile
`Disclosure.c`, `Accessible.c`, and `AccessibleDBus.c` together with `widget.c`,
using the installed public/private headers plus the new disclosure headers and
`-DHAVE_ATSPI_DBUS=1`, then link `Xm`, `Xt`, `X11`, and `dbus-1`. A complete
source-cohort library build is required before release.

Run the native fixture under a real X display or `xvfb-run -a`. Run the separate
external protocol test as:

```
xvfb-run -a dbus-run-session -- python3 tests/disclosure/observe.py \
    /absolute/path/disclosure_test /absolute/path/receipt.json
```

The protocol observer requires the real AT-SPI bus/registry service and `gdbus`.
It verifies registry embedding, names, expandable/expanded state, action-driven
collapse/expansion, focus, and actual expanded-state D-Bus signals. Missing
services fail the test. No in-memory substitute counts as transport acceptance.
