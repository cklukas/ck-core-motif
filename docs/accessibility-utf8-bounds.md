---
nav_exclude: true
---
# Accessibility UTF-8 conversion bounds

The locale-to-UTF-8 accessibility conversion checks both the wide-character
scratch allocation and the four-byte UTF-8 result bound before allocating.
The checks cover both `size_t` arithmetic and Xt's `Cardinal` allocation
interface, and both accepted sizes are passed explicitly at the allocator
boundary, so a representable host size cannot be truncated. Malformed or
unrepresentable input sizes therefore fail cleanly without
wrapping `(count + 1) * sizeof(wchar_t)`. Allocation failures for either
scratch buffer also return cleanly without dereferencing a null pointer.

The source-level regression is
`tests/automated/check-accessible-utf8-bounds.py`.
