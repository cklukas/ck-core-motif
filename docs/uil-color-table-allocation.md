# UIL color-table allocation failures

Color-table semantic actions allocate a table node and an array of color
elements before copying list entries.  Both allocations are now checked.  A
partially allocated table is released and the target receives the existing
semantic error entry instead of a null array dereference.
