---
nav_exclude: true
---
# Mrm literal convenience output validation

The public `MrmFetchIconLiteral`, `MrmFetchBitmapLiteral`, and
`MrmFetchColorLiteral` helpers now validate their caller-owned output pointers
before entering the locked fetch path. The bitmap helper validates its pixmap,
width, and height outputs together; the icon and color helpers validate their
single output. Valid outputs are cleared before the fetch begins, so a failed
lookup cannot leave stale result values visible to the caller.

Previously, a null bitmap width or height was dereferenced directly, while null
icon and color outputs could reach lower-level conversion helpers and crash.
Missing outputs now return `MrmFAILURE` without acquiring locks or allocating a
resource context. Existing successful calls retain their lookup and conversion
behavior.

The source-level regression is
`tests/automated/check-mrm-literal-convenience-outputs.py`.
