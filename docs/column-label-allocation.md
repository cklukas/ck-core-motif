---
nav_exclude: true
---
# Column label allocation handling

When a Column creates its implicit label child, it first builds a generated
name and then creates the label widget. Both the generated-name byte size and
allocation, and child creation, are now checked. Allocation failure returns cleanly
without registering callbacks or dereferencing a null label widget; successful
Column layout behavior is unchanged.

The regression check is `tests/automated/check-column-label-allocation.py`.
