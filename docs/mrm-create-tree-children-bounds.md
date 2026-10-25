# Mrm widget-tree child bounds

`UrmCreateWidgetTree` now checks that a nonzero child-list offset remains inside
the widget record and that the child count stays within `RGMListSizeMax` before
iterating child descriptors. Valid widget trees retain their existing lookup
and creation behavior.

The source-level regression check is
`tests/automated/check-mrm-create-tree-children-bounds.py`.
