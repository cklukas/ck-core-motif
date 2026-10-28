---
nav_exclude: true
---

# UIL database character-table bounds

Allowed-character tables allocate `num_items * num_bits` bytes and store one
pointer per item. The loader now checks the pointer-table capacity, rejects
invalid counts, checks the multiplication and Xt allocation width, and only
then reads and assigns the table pointers.

Regression: `tests/automated/check-uil-db-char-table-bounds.py`
