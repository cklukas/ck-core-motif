---
nav_exclude: true
---
# PNG allocation-failure cleanup

`_XmPngGetImage` owns the decoded PNG buffer until the `XImage` is created.
The destination allocation and `XCreateImage` failure paths now release that
buffer before returning, keeping repeated malformed or resource-exhaustion
loads from accumulating decoded image storage.

`tests/automated/check-png-allocation-cleanup.py` protects both failure paths.
