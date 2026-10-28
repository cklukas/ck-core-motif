---
nav_exclude: true
---

# Clipboard window-search index width

`ClipboardSearchForWindow` now uses an unsigned loop index matching
`XQueryTree`’s `unsigned int` child count, avoiding signed/unsigned traversal
ambiguity while recursively searching descendants.

Regression: `tests/automated/check-clipboard-window-search-index.py`
