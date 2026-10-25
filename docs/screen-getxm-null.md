---
nav_exclude: true
---

# `XmGetXmScreen` null-screen validation

`XmGetXmScreen` now returns `NULL` when called without a `Screen`. This keeps
the compatibility entry point safe before it evaluates `DisplayOfScreen`;
normal screen lookup and implied-screen creation are unchanged.

The focused regression check is
`tests/automated/check-screen-getxm-null.py`.
