---
nav_exclude: true
---

# Command compound-string allocation

`CommandCallback` now checks the result of `XmStringGenerate` before measuring
or delivering the value. If compound-string allocation fails, the callback
returns after releasing the retrieved text instead of dereferencing a null
value. Successful command notifications are unchanged.

The focused source contract is checked by
`tests/automated/check-command-xmstring-allocation.py`.
