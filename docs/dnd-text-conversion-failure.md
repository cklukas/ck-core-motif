---
nav_exclude: true
---

# Text DnD conversion failure

When a Text or TextField drop cannot convert the owner's payload to the
widget's locale, the transfer is now completed with
`XmTRANSFER_DONE_FAIL` before returning. This prevents a conversion failure
from silently leaving the transfer in its default state.

The regression check is
`tests/automated/check-dnd-text-conversion-failure.py`.
