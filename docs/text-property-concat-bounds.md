# Text-property concatenation bounds

When an X text property contains several items, Motif joins them into one
string.  The byte total is now accumulated with `size_t` and checked before
adding each item and the terminating byte.  An impossible allocation returns
`XNoMemory` after releasing the X text list instead of wrapping an `int` and
allocating a buffer that is too small.
