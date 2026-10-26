---
nav_exclude: true
---
# XmText valueWcs initialization termination

When an existing `XmText` source is initialized from `XmNvalueWcs`, the
multibyte conversion now verifies that its result leaves room for the NUL
terminator before writing it.  A full-capacity conversion is rejected instead
of writing beyond the allocation.

Regression coverage: `tests/automated/check-text-value-wcs-init-termination.py`.
