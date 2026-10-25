---
nav_exclude: true
---
# Traversal redirect-cycle fallback

Traversal-control traits can redirect focus repeatedly. Once the bounded
redirect pass limit is reached, `Xm` now keeps the current requested focus
widget instead of asserting and returning `NULL`. This preserves focus
navigation for malformed or mutually redirecting traits while retaining the
existing recursion bound.
