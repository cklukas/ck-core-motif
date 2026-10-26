---
nav_exclude: true
---

# DropDown conversion-size bounds

`XmDropDown` passes the text-field contents to the list converter through an
`XrmValue`. The conversion-size field is Xt/Xrm `Cardinal`-sized, while
`strlen` returns `size_t`. The conversion now rejects text whose byte length
plus its terminating byte cannot be represented, preventing truncation on
wide-size platforms and releasing the retrieved text before returning.

The focused regression check is
`tests/automated/check-dropdown-conversion-size-bounds.py`.
