---
nav_exclude: true
---

# TextField wide-preedit multibyte capacity

When a wide XIM preedit is rendered through a single-byte TextField path, a
character can require multiple locale bytes.  The old temporary buffer used
only `insert_length + 1` bytes and could truncate the conversion.  The buffer
now reserves `MB_CUR_MAX` bytes per input character, checks the `Cardinal`
limit, and handles conversion failure before continuing.

`tests/automated/check-textfield-preedit-multibyte-capacity.py` protects this
Unicode conversion contract.
