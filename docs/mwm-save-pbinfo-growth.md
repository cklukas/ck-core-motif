---
nav_exclude: true
---
# MWM push-button array growth

Menu widget creation grows the array that tracks push buttons.  Growth now
uses a temporary pointer and capacity, checks the increment, byte count, and
Xt's `Cardinal` width, and updates the `MenuSpec` only after `XtMalloc` or
`XtRealloc` succeeds.  A failed growth therefore preserves the existing array and its
size/count state instead of losing the old pointer or resetting unrelated
entries.

`tests/automated/check-mwm-save-pbinfo-growth.py` protects this contract.
