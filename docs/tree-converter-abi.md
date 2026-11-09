---
nav_exclude: true
---
# Tree resource converter ABI

`XmTree`'s connect-style, compress-style, and line-style resource converters
now use the full `XtTypeConverter` signature, including the converter-data
argument. They are registered without incompatible function-pointer casts, so
Xt invokes them through their declared ABI. Resource names, converted values,
and caching behavior are unchanged.

The focused regression check is
`tests/automated/check-tree-converter-abi.py`.
