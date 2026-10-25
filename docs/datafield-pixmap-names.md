---
nav_exclude: true
---
# DataField cursor pixmap names

DataField caches its I-beam and add-mode cursors under names containing the
cursor height and line width.  The old I-beam buffer was only 17 bytes even
though `_XmDataF_%d_%d` needs more space for ordinary 16-bit dimensions; the
unbounded `sprintf` could overwrite the stack when geometry values grew.

Both cursor-name paths now use a 64-byte buffer and bounded `snprintf` writes.
The cache keys and cursor behavior are unchanged for normal values, while
large values are truncated safely rather than corrupting the widget stack.

`tests/automated/check-datafield-pixmap-names.py` protects the storage and
formatting contract.
