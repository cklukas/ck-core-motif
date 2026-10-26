# Vendor-shell modal table growth

`AddGrab` keeps the vendor-shell modal records in a dynamically grown table.
The table is shared by the display's modal-input bookkeeping, so a failed
resize must leave both the old pointer and its capacity unchanged.  Growth
now validates the addition and the byte multiplication against both `size_t`
and the `Cardinal`-sized Xt allocator interface.  It uses a temporary pointer,
publishes the new capacity only after `XtRealloc` succeeds, and returns without
adding the grab when the resize cannot be represented or allocated.

This preserves the existing modal ordering and input semantics while avoiding
an overflow or null dereference under allocation pressure.
