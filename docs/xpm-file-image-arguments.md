---
nav_exclude: true
---

# Validate XPM file-to-image arguments

`XpmReadFileToImage` now rejects a null display, filename, or call with no
image outputs before opening the file or entering Xlib conversion.  Optional
shape-mask-only output remains valid for compatibility.  Invalid public input
returns `XpmFileInvalid` deterministically instead of relying on a downstream
dereference or file-open failure.
