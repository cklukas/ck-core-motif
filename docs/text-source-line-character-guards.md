---
nav_exclude: true
---
# Unicode text-source line access guards

Unicode line scanning now validates source storage, gap ordering, widget
state, character width, and pointer-offset arithmetic before reading a native
character. Invalid or partially destroyed text-source state returns a neutral
character instead of performing undefined pointer arithmetic.


UTF-8 locales can report `MB_CUR_MAX` greater than three; those sources use
`wchar_t` storage and must not be rejected for that width. Gap validation checks
that the gap end is inside the allocation and that its size does not exceed
its end offset. Comparing the gap size with the data capacity after the gap
incorrectly rejects ordinary sources, particularly when the gap is at the end.
`tests/automated/check-text-source-lines.c` verifies native hard-line scans and
counts before and after gap edits in the current UTF-8 locale.
