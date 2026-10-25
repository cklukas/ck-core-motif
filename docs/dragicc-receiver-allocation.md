# Drag receiver state allocation handling

After validating receiver-info data, `_XmGetDragReceiverInfo` allocates the
drop-site state wrapper that owns the X property buffer.  It now checks that
allocation before initialization and releases the property buffer on
failure, returning `False` with no partially initialized receiver state.
