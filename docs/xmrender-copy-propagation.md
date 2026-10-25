---
nav_exclude: true
---
# XmRenderT rendition-copy failure propagation

`_XmRenditionCopy` now checks the result of shared or cloned rendition
allocation before copying scratch state. Allocation failure returns `NULL`
cleanly instead of dereferencing an invalid rendition handle.

`tests/automated/check-xmrender-copy-propagation.py` protects this contract.
