# Cascade button appearance

Modern light and dark profiles give `XmCascadeButton` and
`XmCascadeButtonGadget` flat surfaces, theme selection colors, modern render
tables and density-aware padding. Default padding is 12 by 6 logical pixels
in comfortable density and 8 by 3 in compact density. The default shadow is
zero; application-supplied thicknesses and colors remain authoritative.
The internal resource-ownership query includes live gadgets, using their
Object destruction callbacks to retire state before addresses can be reused.
Arming, disarming and exposure repaint the complete surface so selection does
not leave a legacy bevel or stale gray rectangle.

Generated submenu arrows use clean directional chevrons. Right-to-left menus
reverse their disclosure direction; option-menu gadget arrows point down.
Application-supplied cascade pixmaps retain their existing drawing behavior.
Compound strings, label pixmaps, accelerators and mnemonics continue through the
ordinary Label renderer. Classic and high-contrast profiles retain their
existing drawing paths, and returning to Classic restores toolkit defaults.
No public resources or native instance records change.

Cascade controls belong in a menu RowColumn, such as a menu bar, pulldown or
popup. Use `XmCreateMenuBar`, create the pulldown from that bar, then create the
cascade as a child of the bar with `XmNsubMenuId` referencing the pulldown.
Modern menu bars default to zero outer shadow; explicit application shadows
remain authoritative. The gallery reuses its supplied menu bar; the shared
example factory creates one only when given a generic host. Both host paths
are covered by native tests.

Native qualification uses the actual gallery factory:

```sh
python3 tests/automated/run-cascade-appearance.py \
  --build /absolute/build --results /absolute/fresh-results
```

The test runs on an isolated display and exercises both classes, live palettes,
scale and density changes, selected/unselected pixels, explicit application
resources, action callbacks and repeated destruction/recreation. It sends no
input to the user's desktop.
