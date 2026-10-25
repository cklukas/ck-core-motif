---
nav_exclude: true
---
# Icon-place sentinel handling

`FindIconPlace` now rejects the `NO_ICON_PLACE` sentinel returned by coordinate
conversion before reading the placement array or attempting to grow it.  This
keeps negative or otherwise invalid coordinates from becoming a negative array
index and avoids changing the grid for an invalid request.

`tests/automated/check-find-place-sentinel.py` protects this contract.
