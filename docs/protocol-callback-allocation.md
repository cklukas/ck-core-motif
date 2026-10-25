---
nav_exclude: true
---

# Protocol callback registration

`XmAddProtocolCallback` now checks the protocol lookup again after attempting
to create a missing protocol entry. Allocation failure therefore returns
cleanly instead of dereferencing a null entry; successful callback
registration is unchanged.

The focused regression check is
`tests/automated/check-protocol-callback-allocation.py`.
