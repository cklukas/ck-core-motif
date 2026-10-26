---
nav_exclude: true
---
# Transfer-value target validation

`XmTransferValue` now rejects the `None` atom before creating a transfer
block or incrementing outstanding request counts.  This avoids queuing an
invalid selection request and keeps cancellation/cleanup bookkeeping
balanced.  A null callback remains permitted for internal delete requests.

The focused regression is `tests/automated/check-transfer-value-target.py`.
