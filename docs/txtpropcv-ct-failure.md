---
nav_exclude: true
---

# Compound-text export failures

When exporting an XmString table as COMPOUND_TEXT, a failed per-string
conversion is now reported as `BadAlloc` and previously converted strings are
released. The converter no longer turns a failed conversion into an empty
property element.

The focused regression check is
`tests/automated/check-txtpropcv-ct-failure.py`.
