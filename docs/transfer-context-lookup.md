---
nav_exclude: true
---

# Transfer context lookup

Transfer conversion contexts are keyed by display and selection atom. Their
lookup and insertion now recheck the key while holding `_XmProcessLock`, so
concurrent callers share one context instead of leaking a duplicate. Hash
table allocation and insertion failure also return cleanly without probing a
null table.

The regression check is
`tests/automated/check-transfer-context-lookup.py`.
