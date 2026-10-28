---
nav_exclude: true
---

# XPM token bounds

`xpmNextWord` now rejects tokens that exceed the caller-provided buffer rather
than returning a truncated prefix and leaving the parser in the middle of the
same token. It consumes the remainder for both memory and file inputs, and
rejects null or zero-length destinations safely.

Regression: `tests/automated/check-xpm-token-bounds.py`
