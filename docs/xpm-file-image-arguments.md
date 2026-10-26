---
nav_exclude: true
---

# Validate XPM file-to-image arguments

`XpmReadFileToImage` now rejects a null display, filename, or required image
output pointer before opening the file or entering Xlib conversion.  Optional
shape-mask output remains optional for compatibility.  Invalid public input
returns `XpmFileInvalid` deterministically instead of relying on a downstream
dereference or file-open failure.
