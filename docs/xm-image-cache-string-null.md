---
nav_exclude: true
---
# Xm image-cache string null safety

`_XmInImageCache` now rejects a null image name before hash lookup. The shared
image-name comparator and hash function also tolerate null keys defensively,
while non-null names retain the existing matching and hash behavior.

The source-level regression is
`tests/automated/check-xm-image-cache-string-null.py`.
