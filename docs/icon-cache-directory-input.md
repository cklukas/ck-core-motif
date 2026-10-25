---
nav_exclude: true
---
# IconFile cache directory input guard

`MakeCachedDirEntry` now rejects null and empty directory names before calling
`opendir`. Valid directory names retain the existing cache behavior, while
optional lookup state cannot reach the filesystem API with invalid input.

The focused regression is
`tests/automated/check-icon-cache-directory-input.py`.
