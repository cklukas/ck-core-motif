---
nav_exclude: true
---

# ScrollFrame navigator removal

Navigator removal now checks that a nonempty navigator count has a backing
list before searching it. Inconsistent or partially initialized state is
ignored safely; normal removal and callback cleanup are unchanged.

The focused regression check is
`tests/automated/check-scrollframe-nav-remove.py`.
