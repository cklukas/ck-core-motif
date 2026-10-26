---
nav_exclude: true
---

# TextField overstrike preedit allocations

The TextField overstrike path now checks its remaining-byte and
remaining-wide-character allocations, as well as replacement buffers, before
copying or converting into them. On failure it releases existing temporary
preedit storage and returns without dereferencing NULL.

`tests/automated/check-textfield-preedit-overstrike-allocations.py` protects
the allocation guards.
