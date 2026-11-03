---
nav_exclude: true
---

# Mrm widget-fetch parameters

`MrmFetchWidgetOverride`, and therefore `MrmFetchWidget`, now reject a null
widget index, widget result pointer, or class result pointer.  A nonzero
override count also requires a non-null override argument list.  The entry
point returns the established `MrmFAILURE` result before accessing application
context, acquiring locks, or dereferencing caller-owned pointers. A non-null
`*w_return` remains the documented “already fetched” marker: the call returns
`MrmSUCCESS` without constructing a duplicate widget or changing either
output. For an actual fetch (`*w_return == NULL`), the class output is cleared
before hierarchy lookup, so a failed fetch cannot expose a stale class code;
the widget output remains null until construction succeeds. Successful fetches
retain their established widget and class results.

The focused regression is
`tests/automated/check-mrm-widget-fetch-parameters.py`.
