---
nav_exclude: true
---

# Clipboard format-index comparison

`XmClipboardInquireFormat` rejects non-positive format indexes before looking
up a target. The subsequent atom-count comparison now converts that validated
index to `unsigned long`, avoiding signedness ambiguity without changing the
public API or accepted indexes.

Regression: `tests/automated/check-clipboard-format-index.py`
