---
nav_exclude: true
---
Input-method varargs count bounds
=================================

The input-method varargs helper now checks the counted argument list against
both `size_t` and Xt's `Cardinal` byte limits before calling `XtCalloc`.  This
prevents a large or malformed count from wrapping the temporary `Arg` array
size and keeps the existing null result for invalid or failed allocations.
