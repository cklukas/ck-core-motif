---
nav_exclude: true
---

# Drag ICC receiver count bounds

Receiver drop-site counts now preserve the signed result from the drop-site
manager, reject a negative result, and narrow to the 16-bit ICC property only
after the existing upper-bound check. This prevents an error return or large
local count from becoming a wrapped protocol value.

Regression: `tests/automated/check-dragicc-receiver-count.py`
