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
checks the resulting size against Xt's `Cardinal` width. It skips an
unrepresentable or unavailable cache entry and falls through to normal path
lookup without changing valid cache behavior.

When registering a resolved icon in the cache, the directory component length
is also kept in `size_t` and checked before the Xt allocation. An
unrepresentable directory component skips only the optional cache insertion;
the resolved icon remains usable through the normal lookup result.
