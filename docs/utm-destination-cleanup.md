---
nav_exclude: true
---
# UTM destination cleanup

If the destination callback arrives without an
`XmDestinationCallbackStruct`, the UTM bridge cannot start the transfer. It
now dequeues and frees the pending package in that case. This prevents a later
request from consuming stale parameters and keeps the queue from leaking.

`tests/automated/check-utm-destination-cleanup.py` protects this lifecycle
behavior.
