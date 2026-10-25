---
nav_exclude: true
---
# Wide XmString segment conversion bounds

`XmStringGetNextSegment` converts wide-character segments to the current
locale.  The old allocation divided the maximum multibyte capacity by
`sizeof(wchar_t)`, which could allocate too little space (or zero bytes) for
short segments and silently truncate the result.

The conversion now allocates `char_count * MB_CUR_MAX` bytes with checked
`size_t` and Xt `Cardinal` arithmetic, and rejects results that cannot fit the
API's `short` character-count output. Existing representable segments keep
their exact text and terminator. The source regression in
`tests/automated/check-xmstring-wide-conversion-bounds.py` protects the
capacity and range checks.
