---
nav_exclude: true
---
# WSM match-attribute allocation failure

WSM `WM_WINDOWS` request unpacking now checks the match-attribute pointer
array before indexing it. A failed allocation clears the count and returns,
preventing a null-array write while preserving the existing protocol layout.

`tests/automated/check-wsm-match-attrs-oom.py` protects this failure path.
