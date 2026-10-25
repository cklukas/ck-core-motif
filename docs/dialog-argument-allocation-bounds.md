---
nav_exclude: true
---
# Dialog argument allocation bounds

`XmeCreateClassDialog` appends one internal shell argument to the caller's
argument list. It now rejects counts whose `(count + 1) * sizeof(Arg)` byte
size cannot fit both `size_t` and Xt's `Cardinal` allocator width before
allocating or copying the merged list.

Normal dialog construction and the public argument ABI are unchanged. The
regression is covered by `tests/automated/check-class-dialog-allocation.py`.
