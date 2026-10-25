---
nav_exclude: true
---
# TabBox right-to-left placement

Horizontal right-to-left TabBox layout now computes the initial tab origin in
signed arithmetic.  If the first tab is wider than a collapsed tab box, the
origin remains a valid negative/off-edge coordinate instead of wrapping to a
large positive position.

The source-level regression is `tests/automated/check-tabbox-rtl-placement.py`.
