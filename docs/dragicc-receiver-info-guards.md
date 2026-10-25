# Drag receiver-info property validation

`_XmGetDragReceiverInfo` reads the byte-serialized
`_MOTIF_DRAG_RECEIVER_INFO` property and exposes data and heap views to the
drop-site parser.  It now requires the expected type, 8-bit format, and a
complete X property, then checks that `heap_offset` starts after the header
and ends within the returned bytes.  Invalid properties are rejected before
any pointer arithmetic or parsing.
