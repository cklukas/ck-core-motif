---
nav_exclude: true
---

# Container synthetic getter guards

Container’s detail-header, detail-header-count, and outline-column-width
synthetic getters now validate the widget class and output slot before reading
or rebuilding internal state. Invalid callback inputs return deterministic null
or zero values; valid Container reads retain their existing ownership and
allocation behavior.

The focused source-contract check is
`python3 tests/automated/check-container-synthetic-getter-guards.py`.
