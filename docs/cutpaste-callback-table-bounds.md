# Clipboard callback-table growth bounds

When a clipboard callback table is full, Motif grows both callback tables by
20 entries.  The growth path now rejects integer overflow and checks both the
host `size_t` and Xt `Cardinal` allocation limits before replacing either
table.  Existing rollback cleanup remains unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-cutpaste-callback-table-bounds.py
```
