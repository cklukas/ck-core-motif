---
nav_exclude: true
---

# Clipboard retrieve re-check cleanup

When a cut-by-name callback changes or removes a format, the retrieve path
re-reads that format before copying data.  If the re-check fails, it now closes
the open clipboard header and releases the clipboard lock before returning the
corruption error.  This prevents a failed external-owner transfer from
leaving the clipboard locked.

Regression: `tests/automated/check-clipboard-retrieve-recheck-cleanup.py`
