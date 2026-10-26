---
nav_exclude: true
---
# Mrm legacy callback bounds

Legacy Mrm callback descriptor translation now rejects null descriptors,
negative callback counts, and counts whose descriptor-size arithmetic cannot be
represented by either `size_t` or Xt's `Cardinal` allocation width before
allocating the translated callback record.
Valid 1.1 callback descriptors retain their existing item-copy behavior.

The source-level regression is `tests/automated/check-mrm-callback-bounds.py`.
