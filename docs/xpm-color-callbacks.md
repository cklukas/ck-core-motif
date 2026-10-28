---
nav_exclude: true
---

# XPM color callback attributes

The raw XPM-image conversion path now rejects `XpmAllocColor` and
`XpmFreeColors` attribute flags when their callbacks are null.  This prevents
malformed caller state from reaching a null function-pointer call during color
allocation or cleanup; complete callback configurations retain the existing
custom-color behavior.

Regression: `tests/automated/check-xpm-color-callbacks.py`
