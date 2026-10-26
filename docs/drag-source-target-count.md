---
nav_exclude: true
---
# Drag-source target-count conversion

The export-target converter reports its result length as an `unsigned long`,
while `XmNnumExportTargets` is a `Cardinal` resource.  `XmeDragSource` now
rejects counts that cannot be represented and performs an explicit `Cardinal`
conversion for accepted counts.  This avoids silent truncation on 64-bit
systems while preserving the existing drag-source API.

The focused regression is
`tests/automated/check-drag-source-target-count.py`.
