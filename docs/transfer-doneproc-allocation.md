# Transfer completion-callback allocation

`XmeTransferAddDoneProc` grows the callback array while a transfer context is
locked. The count and pointer now change only after the new allocation
succeeds, and null or unrepresentable contexts are ignored safely. A failed
growth therefore leaves all previously registered callbacks intact and avoids
a null write during transfer teardown.

Successful registrations retain their original order and callback semantics.
