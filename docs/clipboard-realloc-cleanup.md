---
nav_exclude: true
---

# Clipboard assembly allocation cleanup

`XmClipboardRetrieve` assembles an incremental X selection into an Xt-owned
buffer.  Growth uses a temporary pointer for `XtRealloc`: if allocation fails,
the existing buffer remains reachable by the common failure path and is freed.
This avoids leaking a partially assembled selection while preserving the
existing success and failure return contracts.

Regression: `tests/automated/check-clipboard-realloc-cleanup.py`
