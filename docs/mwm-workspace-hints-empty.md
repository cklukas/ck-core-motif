---
nav_exclude: true
---

# Empty MWM workspace hints

An explicit workspace-hints property may contain an empty workspace list.  The
reader now returns that as a successful empty result instead of allocating zero
bytes and depending on allocator-specific `XtMalloc(0)` behavior.

Regression: `tests/automated/check-mwm-workspace-hints-empty.py`.
