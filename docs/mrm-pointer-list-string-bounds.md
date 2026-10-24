# Mrm pointer-list string bounds

Pointer-list string appends now keep the source length in `size_t`, reject an
unrepresentable terminator-inclusive allocation, and copy the validated bytes
directly. Valid appends retain their existing ownership and list behavior;
malformed sizes return the existing `MrmFAILURE` path safely.

The source-level regression is
`tests/automated/check-mrm-pointer-list-string-bounds.py`.
