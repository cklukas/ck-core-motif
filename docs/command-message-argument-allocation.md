---
nav_exclude: true
---

# Command and message-dialog argument allocation bounds

The `XmCreateCommand` and message-dialog convenience constructors append an
internal `XmNdialogType` argument before creating the widget. Their public
counts are legacy `Cardinal` or `int` values, while the temporary argument
array is allocated through Xt's byte-counted allocator. The constructors now
reject counts that cannot represent both the extra element and the resulting
byte size in host `size_t` or Xt `Cardinal` before multiplying. Empty argument
lists remain valid when their pointer is null; non-empty lists require a valid
source pointer.

The temporary array is checked before copying or appending the internal
argument; allocation failure returns `NULL` without modifying caller-owned
arguments.

The focused source regression is
`tests/automated/check-command-message-argument-bounds.py`.
