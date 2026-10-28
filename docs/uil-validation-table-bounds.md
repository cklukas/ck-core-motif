---
nav_exclude: true
---

# UIL validation-table bounds

The semantic validator allocates one entry for every known argument and
callback reason. These counts originate in the parsed UIL symbol tables and
must not be used directly in a multiplication or narrowed to Xt's `Cardinal`
allocation size. The validator now rejects negative or overflowing counts,
checks the allocation size against `UINT_MAX`, and reports allocation failure
before walking the parse tree.

Regression: `tests/automated/check-uil-validation-table-bounds.py`
