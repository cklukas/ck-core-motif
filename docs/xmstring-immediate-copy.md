---
nav_exclude: true
---
# XmString immediate-segment copies

Immediate optimized segments store exactly their declared `byte_count` bytes;
the compact representation does not reserve a trailing C-string terminator.
The old `_XmEntryTextSet` path used `strcpy`, which could write one byte past a
compact allocation and treated wide-character payloads as narrow strings.

The setter now copies exactly the declared byte count with `memcpy`.  This
preserves the existing representation and supports both narrow and wide text
without an overrun.

The regression check is `tests/automated/check-xmstring-immediate-copy.py`.
