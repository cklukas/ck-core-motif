---
nav_exclude: true
---
# XmToggleButton state API null-safety

`XmToggleButtonGetState`, `XmToggleButtonSetState`, and
`XmToggleButtonSetValue` now reject null widgets before Xt context lookup or
gadget dispatch. Valid widget and gadget behavior, including callbacks, remains
unchanged.
