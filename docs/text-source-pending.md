# Text-source pending snapshots

Pending-state snapshots now validate the widget count and allocator-size
arithmetic before allocating their Boolean array.  Allocation failure returns
`NULL` without entering the fill loop, avoiding a null dereference while
preserving the existing caller contract for successful snapshots.

They also reject a missing source or widget array before reading the count, so
a snapshot racing teardown fails safely instead of dereferencing torn-down
state.

The focused source contract is checked by
`tests/automated/check-text-source-pending.py`.
