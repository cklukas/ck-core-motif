---
nav_exclude: true
---
# CascadeButton resource-table bounds

CascadeButton class initialization checks the combined superclass and subclass
resource counts before allocating the merged `XtResource` table, and checks the
allocation before populating it. An unrepresentable or unavailable table
leaves initialization without wrapping its count or dereferencing null state;
normal resource merging is unchanged.

The source-level regression is
`tests/automated/check-cascade-resource-bounds.py`.
