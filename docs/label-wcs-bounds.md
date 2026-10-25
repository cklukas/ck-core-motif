# Label wide-string conversion bounds

The wide-character `XmLabel` value path converts a `wchar_t` value with
`wcstombs` before creating the localized compound string. The conversion
needs room for the terminating NUL even when the source is empty. The helper
now allocates `MB_CUR_MAX * length + 1` bytes and rejects multiplication
overflow before allocating, preserving the existing label behavior for valid
values while preventing an out-of-bounds terminator write.

`tests/automated/check-label-wcs-bounds.py` guards the allocation contract.
