# Dynamic representation-type table allocation

Dynamic representation types are appended to a shared table.  Before growing
that table, the count-to-byte calculation now checks both `size_t` and Xt's
`Cardinal` allocator range.  A failed `XtRealloc` leaves the old table intact
and returns the invalid representation-type sentinel as before.
