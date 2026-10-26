# Icon-directory cache accumulation bounds

The icon-directory cache stores directory entries in a fixed 64 KiB staging
buffer. Its entry-length and accumulated-offset arithmetic now stays in
`size_t` and checks the remaining capacity before copying a name. This avoids
narrowing a `strlen` result to `int` before the bound check.

The focused source regression is
`tests/automated/check-icon-cache-directory-bounds.py`.
