# Xmos directory-entry list allocation

Directory scans grow the caller-owned entry-pointer array in fixed blocks and
allocate one string per accepted candidate. Growth now publishes the new
capacity only after `XtRealloc` succeeds, and both full-path and basename
string allocations are checked.

On memory pressure the scan stops with all previously collected entries and
their original capacity intact; successful enumeration is unchanged.
