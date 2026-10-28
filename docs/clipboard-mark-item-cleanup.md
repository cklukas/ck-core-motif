---
nav_exclude: true
---

# Clipboard mark-item cleanup

`ClipboardMarkItem` now frees the Xt-owned data-item buffer after writing the
updated delete-pending state back to the clipboard property.  Error cleanup
was already present; successful mark operations no longer leak their
retrieved record.

Regression: `tests/automated/check-clipboard-mark-item-cleanup.py`
