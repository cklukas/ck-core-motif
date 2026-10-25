---
nav_exclude: true
---
# Shared XmRenderT handle allocation

Shared rendition copies now check their handle allocation before publishing the
shared pointer. On failure, the temporary reference increment is undone and
`NULL` is returned; successful sharing is unchanged.

`tests/automated/check-xmrender-shared-handle-allocation.py` protects this
contract.
