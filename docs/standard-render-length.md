---
nav_exclude: true
---
# Standard render-table conversion length

`XmRenderTableCvtToProp` returns an `unsigned int` byte length. The standard
converter now preserves that width until assigning the selection callback's
length, avoiding a narrowing conversion on large render-table properties.

The focused regression is
`tests/automated/check-standard-render-length.py`.
