---
nav_exclude: true
---
# FontSelector encoding-list ownership

`XmFontSelector` now allocates room for both every copied encoding pointer and
the required null terminator. The previous allocation omitted that terminator
slot, so an otherwise ordinary non-empty `XmNencodingList` wrote one pointer
beyond its allocation during initialization.

An empty or null encoding list is now supported as an explicit, useful state:
the encoding menu retains its `Any` choice, menu rebuilding does not dereference
a null list, and selecting `Any` remains possible. Encoding lists are also
bounded by the allocation and signed menu-index ranges before copying.

A null `XmNdefaultEncodingString` is likewise normalized to `Any` during
construction and runtime updates. Allocation failure preserves the previous
valid selection rather than introducing a null value.

The focused source contract is covered by
`tests/automated/check-fontselector-encoding-list.py`.
