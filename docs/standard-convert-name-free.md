---
nav_exclude: true
---
# Standard conversion WM_NAME ownership

The `WM_NAME` property returned by `XGetWindowProperty` is owned by Xlib. The
standard `XA_WM_NAME` conversion now releases that property after converting
it to the toolkit-owned locale string, including conversion-failure paths.

Regression coverage: `tests/automated/check-standard-convert-name-free.py`.
