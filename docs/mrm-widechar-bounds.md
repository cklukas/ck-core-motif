---
nav_exclude: true
---
# Mrm wide-character bounds

Mrm wide-character value fixup now rejects negative serialized byte counts
before converting them to `size_t`, and checks the terminating wide-character
allocation against `SIZE_MAX`. Valid wide-character values retain their
existing conversion and storage behavior.

The source-level regression is `tests/automated/check-mrm-widechar-bounds.py`.
