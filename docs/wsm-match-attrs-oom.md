---
nav_exclude: true
---
# WSM match-attribute allocation failure

WSM `WM_WINDOWS` request unpacking now checks both the match-attribute pointer
array and each nested property-array count against Xt's `Cardinal` width before
indexing or allocating. A failed or unrepresentable allocation clears the
count and returns, preventing a null-array write while preserving the existing
protocol layout.

`tests/automated/check-wsm-match-attrs-oom.py` protects this failure path.
