---
nav_exclude: true
---
# DataField warning formatting

`XmDataField` reports characters that cannot be represented by the active font.
Those diagnostics include localized message text and, for wide-character input,
a converted string. They are now sized with `snprintf` before allocation, so a
long translation cannot overwrite a fixed stack buffer. Allocation failure in
the formatter is still handled by the normal Xt allocator contract, and a
formatting failure falls back to the unformatted catalog message.

The source-contract regression is
`tests/automated/check-datafield-warning-bounds.py`. It verifies that all
validation warning paths use the bounded helpers and that the former fixed
buffer and unbounded `sprintf` calls are absent.
