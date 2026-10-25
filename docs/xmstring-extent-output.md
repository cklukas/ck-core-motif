---
nav_exclude: true
---
# XmStringExtent output-pointer guards

`XmStringExtent` now validates its `width` and `height` output pointers before
writing them. A call with either pointer missing returns safely; valid calls
still receive the same dimensions, and non-null outputs are initialized to zero
before other inputs are checked.

The regression check is
`tests/automated/check-xmstring-extent-output.py`.
