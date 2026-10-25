---
nav_exclude: true
---
# Mrm widget-reference storage bounds

Widget references store names in a 16-bit resource-context layout. The
construction path now measures names in `size_t`, rejects names beyond the
format limit before narrowing, checks full-word alignment arithmetic, and
avoids subtracting a larger used-size value from the context capacity.
Valid references retain their existing lookup and ownership behavior; an
unrepresentable name returns `MrmTOO_MANY` safely.

The source-level regression is
`tests/automated/check-mrm-wref-storage-bounds.py`.
