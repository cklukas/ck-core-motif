---
nav_exclude: true
---
# ColorSelector mode lifecycle

The selected mode toggle is an optional internal child.  ColorSelector now
keeps initialization and later resource changes safe when the radio box or a
toggle cannot be created: mode selection still updates the widget state, while
the toggle-state update is skipped for a missing child.

The mode callback also rejects incomplete callback data before reading its
reason or set fields.  Fully constructed selectors retain the existing mode
switching and callback behavior.
