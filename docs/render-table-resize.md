---
nav_exclude: true
---
# Render-table resize

`XmRenderTableRemoveRenditions` now plans retained entries before changing the
live table. Unique tables keep their public handle and resize only after the
new body is available; shared tables receive a separate body whose rendition
handles are copied before the old handle is released. A failed allocation or
copy returns the original table without publishing a partial compaction.
