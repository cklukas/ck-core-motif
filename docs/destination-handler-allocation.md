# Destination handler allocation

Destination transfers now check the callback structure, pooled transfer
context, and drag-and-drop callback copy before initialization. Failed setup
returns `False` after releasing any partial transfer state, while
`GetTransferID` leaves its pool unchanged when a new context cannot be
allocated.

Successful selection and DnD transfers retain their existing callback and
ownership behavior.
