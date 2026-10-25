# XmString optimized-entry copy bounds

Optimized XmString entries can carry inline text whose byte count is stored
in the entry.  The copy path previously formed the allocation size in `int`
and then copied through unchecked allocation results.  Large values could
wrap before the allocation or cause a null-pointer copy after allocation
failure.

The path now computes the size in `size_t`, rejects values that exceed Xt's
`Cardinal` allocation contract, and releases a partially built entry when
out-of-line text allocation fails.
