---
nav_exclude: true
---
# ColorObj read-side storage validation

`_XmGetColor` now requires the per-screen color-use and color-set arrays to
exist before reading them. A partially initialized ColorObj therefore returns
failure instead of indexing missing storage; valid running ColorObj reads are
unchanged.

`tests/automated/check-color-read-storage.py` protects this contract.
