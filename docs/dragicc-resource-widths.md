---
nav_exclude: true
---

# Drag ICC receiver resource widths

Receiver drag-site resources now enter their 32-bit ICC fields explicitly.
Pixmap-depth serialization rejects values above the protocol's 16-bit range
before writing the property, preventing silent truncation while preserving the
existing property layout.

Regression: `tests/automated/check-dragicc-resource-widths.py`
