---
nav_exclude: true
---

# Mrm widget-fetch parameters

`MrmFetchWidgetOverride`, and therefore `MrmFetchWidget`, now reject a null
widget index, widget result pointer, or class result pointer.  A nonzero
override count also requires a non-null override argument list.  The entry
point returns the established `MrmFAILURE` result before accessing application
context, acquiring locks, or dereferencing caller-owned pointers.  Once both
result pointers have been validated, it clears them before hierarchy lookup or
widget construction, so every failure leaves `*w_return == NULL` and
`*class_return == 0` rather than exposing stale caller state.  Successful
fetches retain their established widget and class results.

The focused regression is
`tests/automated/check-mrm-widget-fetch-parameters.py`.
