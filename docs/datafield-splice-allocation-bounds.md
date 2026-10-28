# DataField splice allocation bounds

When a DataField modify-verify callback builds a replacement string, the
splice size is passed to Xt's `Cardinal`-sized allocator.  The splice path now
rejects sizes above `UINT_MAX` as well as ordinary `size_t` addition overflow,
so a 64-bit size cannot silently truncate before allocation.

Normal edits are unchanged; an unrepresentable edit is rejected through the
existing `doit = False` path.
