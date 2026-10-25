---
nav_exclude: true
---
# Pixmap-cache input validation

`_XmCachePixmap` queries geometry immediately when dimensions are omitted.
Passing a null screen or the unspecified pixmap handle therefore reached Xlib
with invalid arguments.

The API now rejects either invalid handle before initializing cache state or
issuing X requests.  Existing valid pixmap caching and geometry behavior are
unchanged.

`tests/automated/check-cache-pixmap-input.py` protects this boundary.
