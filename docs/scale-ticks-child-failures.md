---
nav_exclude: true
---
# Scale tick child-creation failures

`XmScaleSetTicks` now increments its managed-child count only when a separator
gadget was created successfully. A failed child creation is therefore omitted
from `XtManageChildren` instead of passing a NULL widget through the batch.

The focused regression is
`tests/automated/check-scale-ticks-child-failures.py`.
