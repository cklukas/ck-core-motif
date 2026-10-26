# Text modify-verify allocation guards

The text source copies callback payloads into temporary character and
wide-character buffers before invoking modify-verify callbacks.  Those
allocations are now checked before copying or conversion.  An allocation or
size failure aborts the edit cleanly and releases any temporary character
buffer already owned by the operation, preventing null dereferences and
partially initialized callback data.

The focused source contract is checked by
`tests/automated/check-text-verify-allocation-guards.py`.
