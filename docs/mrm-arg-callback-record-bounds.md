# Mrm callback argument-record bounds

Callback-valued widget arguments now validate their complete legacy or current
serialized callback descriptor before translation and fixup. This applies the
same record-size and item-count contract used by creation callbacks, preventing
truncated callback values from reaching conversion while preserving valid
callback argument behavior.

The source-level regression check is
`tests/automated/check-mrm-arg-callback-record-bounds.py`.
