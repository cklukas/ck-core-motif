# Drag target-table property validation

`ReadTargetsTable` reads `_MOTIF_DRAG_TARGETS` as a byte-serialized sequence
of target-list counts followed by 32-bit atoms.  The reader now verifies the
property type and format and walks every advertised list to ensure its count
and atom payload fit in the bytes returned by Xlib.  Truncated or malformed
cache data is rejected before the existing table parser advances through it.
