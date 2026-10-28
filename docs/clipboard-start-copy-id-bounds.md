---
nav_exclude: true
---

# Clipboard start-copy ID bounds

`XmClipboardStartCopy` now rejects nonpositive IDs returned by the internal ID
allocator and explicitly converts the validated label ID into the serialized
unsigned field. A negative allocator result can no longer wrap into a large
clipboard record ID.

Regression: `tests/automated/check-clipboard-start-copy-id-bounds.py`
