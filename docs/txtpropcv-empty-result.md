---
nav_exclude: true
---

# Empty text-property results

An empty X text property is a valid conversion result. The single-item helper
now returns an allocated empty string when Xlib reports zero items, rather than
leaving the caller's output pointer uninitialized while reporting success.

The focused regression check is
`tests/automated/check-txtpropcv-empty-result.py`.
