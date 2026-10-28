# Deferred clipboard request count locking

Deferred clipboard targets publish a data-id-to-snapshot association and then
track the number of outstanding callbacks. These operations now occur under
one `_XmProcessLock` critical section. Previously a callback could run after
the hash insertion but before `outstanding` was incremented, reject the
request as inactive, and lose the deferred conversion. Valid single- and
multi-target transfers retain their existing behavior.

Regression coverage: `tests/automated/check-clipboard-request-count-lock.py`.
