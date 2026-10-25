---
nav_exclude: true
---
# Mrm unknown argument-tag string bounds

Unknown argument tags now require a valid, NUL-terminated string inside the
widget record before conversion, deferred-reference handling, or final Xt
argument construction uses the tag offset.  Compressed known tags retain their
existing lookup path, and valid unknown tags remain compatible.

The source-level regression is
`tests/automated/check-mrm-arg-tag-string-bounds.py`.
