---
nav_exclude: true
---

# Clipboard inquiry name-length width

`XmClipboardInquireCount` now keeps the result of `strlen` in `size_t` while
computing the maximum format-name length. This avoids narrowing a source
length to `int` before comparing it with the unsigned API result.

Regression: `tests/automated/check-clipboard-inquire-count-length-width.py`
