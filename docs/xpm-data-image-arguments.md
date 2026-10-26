---
nav_exclude: true
---

# Validate XPM image-to-data arguments

`XpmCreateDataFromImage` now rejects a null display, output-array pointer, or
both image inputs before conversion.  A shape-only conversion remains valid,
matching the lower-level image scanner's existing behavior.
