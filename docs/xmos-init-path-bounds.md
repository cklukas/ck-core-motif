---
nav_exclude: true
---
# Xmos default resource path bounds

`_XmOSInitPath` expands the default and `XAPPLRES` resource search templates
with home, installation, and include directories.  The old code estimated
those allocations by multiplying string lengths before calling `sprintf`.

Both templates now use a measured `snprintf` pass, exact allocation, and a
bounded second pass.  Search order and template contents are unchanged.

`tests/automated/check-xmos-init-path-bounds.py` protects both branches.
