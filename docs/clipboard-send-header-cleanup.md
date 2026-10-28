---
nav_exclude: true
---

# Clipboard request header cleanup

The data-request branch of `ClipboardSendMessage` now frees the retrieved
clipboard header after persisting its recopy ID.  Invalid-header cleanup was
already present; valid request messages no longer leak the temporary header
allocation.

Regression: `tests/automated/check-clipboard-send-header-cleanup.py`
