---
nav_exclude: true
---
# Transfer selection-callback inputs

Selection callbacks can race cancellation and widget destruction.  The
bookkeeping wrapper now validates the callback widget, transfer context,
selection/type pointers, and length/format outputs before reading the request
queue or interned atoms.  Invalid callback state is ignored safely instead of
dereferencing a stale transfer context.

The focused regression is
`tests/automated/check-selection-callback-inputs.py`.
