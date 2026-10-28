---
nav_exclude: true
---

# Drag bootstrap property validation

The Motif drag bootstrap readers now reject truncated X properties and require
non-null returned buffers before reading window, atom-table, target-table, or
proxy-window data.  This keeps malformed or oversized root/window properties
on the existing recovery path instead of dereferencing incomplete data.

Regression: `tests/automated/check-drag-bootstrap-property-validation.py`
