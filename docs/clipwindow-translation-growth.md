# Clip-window translation-buffer growth

The scrolled-window translation builder grows a shared buffer while composing
virtual-key bindings.  The required string length is checked against
`UINT_MAX`; growth now also stops doubling before a capacity would exceed that
Xt allocator limit.  It falls back to the exact required size and leaves the
old buffer intact when `XtRealloc` fails.

This avoids a 64-bit size being narrowed by an Xt implementation that accepts
`Cardinal` allocation sizes, without changing generated translations.
