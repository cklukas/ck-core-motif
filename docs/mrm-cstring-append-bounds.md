---
nav_exclude: true
---
# Mrm compound-string append bounds

Mrm compound-string appends now retain the encoded byte length in `size_t` and
reject encoded data larger than `MrmMaxResourceSize` before passing it to the
16-bit record allocator. Empty and valid compound strings keep their existing
behavior.

The source-level regression is `tests/automated/check-mrm-cstring-append-bounds.py`.
