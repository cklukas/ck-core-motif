# Render-table property export bounds

`XmRenderTableCvtToProp` serializes tags, font names, tab lists, and rendition
attributes into an X11 property string. Its old append macro doubled the output
buffer only once, so a single long field could still overflow it. It also used
fixed 2048-byte temporary buffers and appended a newline using the previous
field's length, which could read beyond the newline literal.

The exporter now grows its output until the complete field fits with overflow
checks, uses bounded formatting for temporary records and tab lists, and appends
the newline with its actual length. Oversized or unrepresentable output fails
cleanly instead of corrupting memory.

`tests/automated/check-render-table-export-bounds.py` guards these contracts.
