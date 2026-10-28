---
nav_exclude: true
---

# Clipboard retrieve format cleanup

When `XmClipboardRetrieve` finds a format in the local clipboard, it updates
the format record in the property after copying data.  The temporary record is
now released immediately after that replacement, preventing a leak on every
successful local retrieve.

Regression: `tests/automated/check-clipboard-retrieve-format-cleanup.py`
