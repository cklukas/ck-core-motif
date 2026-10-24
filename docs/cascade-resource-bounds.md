# CascadeButton resource-table bounds

CascadeButton class initialization checks the combined superclass and subclass
resource counts before allocating the merged `XtResource` table. An
unrepresentable table leaves initialization without wrapping its count or byte
size; normal resource merging is unchanged.

The source-level regression is
`tests/automated/check-cascade-resource-bounds.py`.
