---
nav_exclude: true
---
# Text-property list concatenation allocation

When an X text property yields multiple strings,
`TextPropertyToSingleTextItem` joins them into one result.  Result buffers are
now checked against both host `size_t` and Xt's `Cardinal` byte range before
initialization; allocation failure releases the Xlib string list and reports
`XNoMemory` to the caller.

The regression check is
`tests/automated/check-txtpropcv-concat-allocation.py`.
