# Mrm widget-tree child index bounds

`UrmCreateWidgetTree` now validates the complete serialized child descriptor
region before reading its count or iterating descriptors. Index children must
point into the widget record and contain a terminating NUL before lookup
helpers receive the string. Malformed records release the temporary child
context before returning, while valid resource files keep the existing public
and private lookup behavior.

The source-level regression check is
`tests/automated/check-mrm-create-tree-child-index-bounds.py`.
