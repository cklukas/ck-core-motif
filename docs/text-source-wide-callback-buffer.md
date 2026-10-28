# Wide text-source callback buffers

The text-source modify-verify bridge allocates a wide callback buffer before
converting a multibyte callback result.  It now writes a wide NUL on conversion
failure and after successful conversion, so callbacks always receive a valid
empty or terminated buffer alongside the reported length.
