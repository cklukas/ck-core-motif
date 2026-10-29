---
nav_exclude: true
---
# ScrollBar value API inputs

`XmScrollBarGetValues` and `XmScrollBarSetValues` now reject both `NULL` and
non-ScrollBar widget handles before obtaining an application context or reading
ScrollBar instance fields.  Passing an unrelated widget therefore fails
quietly instead of interpreting its private data as scrollbar state.

The valid-widget behavior, resource model, callbacks, and ABI remain
unchanged.  The focused source regression is
`tests/automated/check-scrollbar-api-inputs.py`.
