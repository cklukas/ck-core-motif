# Sized icon-name construction bounds

Icon lookup appends the requested size suffix to an image name before probing
its cache and search paths. The suffix path now keeps lengths in `size_t`,
checks the combined allocation size, and treats allocation failure as a normal
lookup miss. Existing valid icon names and cache behavior are unchanged.

The source-level regression is
`tests/automated/check-icon-suffix-bounds.py`.
