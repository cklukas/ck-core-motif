---
nav_exclude: true
---

# Clipboard format deletion cleanup

`ClipboardDeleteFormats` now releases the owning data-item buffer when a
format header cannot be retrieved or is too short.  Successful deletion keeps
the existing per-format cleanup and record removal behavior; malformed
format records no longer leak the outer clipboard item allocation.

Regression: `tests/automated/check-clipboard-delete-formats-cleanup.py`
