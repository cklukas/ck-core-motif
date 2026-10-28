---
nav_exclude: true
---

# UIL compound-string component length bounds

`XmStringComponentCreate` accepts an unsigned-int length, while UIL obtains
component lengths from `strlen`. Both locale and text component paths now
check that conversion before calling the public Motif API.

Regression: `tests/automated/check-uil-component-length-bounds.py`
