---
nav_exclude: true
---

# Secondary transfer lock failure

Secondary selection conversion is synchronous and uses a process-level lock
while it dispatches the destination transfer.  If `_XmDestinationHandler`
rejects that transfer, the conversion callback now invokes
`ReleaseSecondaryLock` before returning `XmCONVERT_REFUSE`.  Without this
cleanup, one rejected request left the lock set permanently and caused every
subsequent secondary conversion to be refused.

The regression check is `tests/automated/check-secondary-lock-failure.py`.
