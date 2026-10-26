# Compound-text stack-copy length

The UTF-8 and locale compound-text converters copy each text component to a
NUL-terminated temporary buffer through `XmStackAlloc`.  Because the component
length is unsigned, `len + 1` wrapped at `UINT_MAX` and could produce a zero
allocation before the copy.  Both conversion paths now reject that impossible
NUL-terminated size before allocation.
