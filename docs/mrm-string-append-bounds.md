# Mrm string append bounds

Mrm ordinary string appends now keep `strlen` results in `size_t` and reject
terminator-inclusive strings that exceed `MrmMaxResourceSize` before passing the
length to the 16-bit record allocator. Valid strings retain their existing
storage and offset behavior.

The source-level regression is `tests/automated/check-mrm-string-append-bounds.py`.
