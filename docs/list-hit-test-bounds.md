---
nav_exclude: true
---

# List hit-test bounds

List hit-testing now compares the event Y coordinate with the lower content
boundary using signed `long long` arithmetic.  A tall list can no longer turn
`core.height - BaseY` into a negative value through a `Position` cast and
misclassify clicks near the bottom.

`tests/automated/check-list-hit-test-bounds.py` protects this contract.
