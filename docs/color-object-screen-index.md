---
nav_exclude: true
---
# Color-object pixel-data screen validation

`FetchPixelData` now rejects a missing widget, missing per-screen storage, and
screen indices outside the ColorObj’s range before parsing property data or
writing the result. Valid screen updates remain unchanged.

`tests/automated/check-color-object-screen-index.py` protects this contract.
