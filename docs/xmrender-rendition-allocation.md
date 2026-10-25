---
nav_exclude: true
---
# XmRenderT rendition allocation

Rendition cloning and renewal now check record allocation before clearing or
copying the record. Cloning also checks its handle allocation and releases the
record if the handle cannot be created, preserving safe failure behavior for
render-table updates.

`tests/automated/check-xmrender-rendition-allocation.py` protects this
contract.
