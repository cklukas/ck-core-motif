---
nav_exclude: true
---
# Sized icon-name construction bounds

Icon lookup appends the requested size suffix to an image name before probing
its cache and search paths. The suffix path now keeps lengths in `size_t`,
checks both the combined allocation size and Xt's `Cardinal` width, and treats
an unrepresentable or failed allocation as a normal lookup miss. Existing valid
icon names and cache behavior are unchanged.

The source-level regression is
`tests/automated/check-icon-suffix-bounds.py`.

Cache-entry path joining applies the same checked `size_t` arithmetic and
falls through to normal path lookup if the joined buffer cannot be allocated.
