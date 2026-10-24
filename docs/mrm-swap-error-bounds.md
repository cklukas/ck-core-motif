# Mrm byte-swap diagnostic bounds

Mrm byte-swapping reports malformed record, callback, and widget-resource
types through fixed 300-byte diagnostic buffers. Those paths now use
`snprintf` with the destination size, preserving the existing error messages
while preventing an unexpectedly large record value from overflowing the
stack buffer.

The source-level regression is
`tests/automated/check-mrm-swap-error-bounds.py`.
