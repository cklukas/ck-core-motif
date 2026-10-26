---
nav_exclude: true
---
# Drop destination argument count

The drop-destination setup passes its argument count to Xt APIs, whose public
type is `Cardinal`. The local counter now uses that type, avoiding a signed
narrowing diagnostic while preserving the same one-argument setup.

The focused regression is
`tests/automated/check-transfer-destination-arg-count.py`.
