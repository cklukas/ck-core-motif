# XmString table unparse allocation failure

`XmStringTableUnparse` allocates an output pointer table before converting
each XmString. The bounded byte size is passed explicitly as an Xt `Cardinal`
while the Motif process lock is held. If it fails, the lock is released and
`NULL` is returned without writing through invalid storage.

The regression check is
`tests/automated/check-xmstring-table-unparse-allocation.py`.
