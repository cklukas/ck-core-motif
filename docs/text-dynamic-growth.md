# Text repaint and highlight growth

Text editing grows two dynamic arrays while processing input: repaint ranges
and highlight records.  Their `XtRealloc` results are now held in temporary
pointers and checked before the widget publishes the new capacity or writes a
new record.  Counter updates happen only after successful growth, so allocator
failure leaves the previous valid array and its bookkeeping intact.

The focused source contract is checked by
`tests/automated/check-text-dynamic-growth.py`.
