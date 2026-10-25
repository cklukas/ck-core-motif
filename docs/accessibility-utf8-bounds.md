# Accessibility UTF-8 conversion bounds

The locale-to-UTF-8 accessibility conversion checks both the wide-character
scratch allocation and the four-byte UTF-8 result bound before allocating.
Malformed or unrepresentable input sizes therefore fail cleanly without
wrapping `(count + 1) * sizeof(wchar_t)`. Allocation failures for either scratch buffer also return cleanly without dereferencing a null pointer.

The source-level regression is
`tests/automated/check-accessible-utf8-bounds.py`.
