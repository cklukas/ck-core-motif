---
nav_exclude: true
---
# XmRenderT scratch-tag copy allocation

Cloned renditions now check scratch-tag array allocation before copying tags.
If it fails, the partial rendition and handle are released and the copy
returns `NULL`, preserving atomic render-state updates.

`tests/automated/check-xmrender-tag-copy-allocation.py` protects this contract.
