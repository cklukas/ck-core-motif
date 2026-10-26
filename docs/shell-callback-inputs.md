---
nav_exclude: true
---
# Shell title and callback input guards

`XmeSetWMShellTitle`, `XmeAddFocusChangeCallback`, and
`XmeRemoveFocusChangeCallback` now return for a null widget before
application-context lookup. Valid shell title updates and vendor focus
callback operations are unchanged.

The focused regression is
`tests/automated/check-shell-callback-inputs.py`.
