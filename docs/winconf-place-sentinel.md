---
nav_exclude: true
---
# Window-configuration placement sentinel guard

The icon move path in `WmWinConf.c` now converts negative, oversized, or
storage-free placement results to `NO_ICON_PLACE` before checking occupancy.
This prevents invalid coordinates from becoming a placement-list index.

`tests/automated/check-winconf-place-sentinel.py` protects the contract.
