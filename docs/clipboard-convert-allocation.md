# Clipboard conversion allocation handling

The clipboard selection conversion path now checks the data buffer length
against Xt's `Cardinal` allocation width before allocating storage for
non-built-in targets. Allocation failure or an oversized length returns a
failed conversion with no output buffer. If retrieval fails after allocation,
the temporary buffer is released and the output pointer and size are cleared,
preventing a leak and avoiding exposure of an invalid result.
