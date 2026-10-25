---
nav_exclude: true
---
# XmOS filename allocation

Home-directory caching and `_XmOSBuildFileName` now check their allocated
buffers before copying. A failed home-directory copy falls back to the
existing empty-name behavior, while filename construction returns `NULL`
without changing successful absolute or relative path handling.

`tests/automated/check-xmos-filename-allocation.py` protects this contract.
