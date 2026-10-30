---
nav_exclude: true
---

# InfoBar close icon geometry

The InfoBar close control now draws a centered, symmetric framed X. Both
diagonals use the same four-pixel inset around the 18-pixel button center, so
the icon remains balanced at every supported bar size.

The focused source contract is checked by
`tests/automated/check-infobar-close-icon.py`.
