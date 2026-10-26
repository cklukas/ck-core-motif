---
nav_exclude: true
---

# DnD failure returns

Destination callbacks reject transfers when their widget is insensitive. They
now return immediately after `XmTransferDone(...XmTRANSFER_DONE_FAIL)`,
because completion can release the transfer callback structure and invoke
reentrant cleanup. Continuing to inspect that structure after completion was
a use-after-free risk.

The regression check is
`tests/automated/check-dnd-failure-returns.py`.
