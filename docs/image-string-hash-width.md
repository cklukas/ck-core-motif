---
nav_exclude: true
---
# Image-cache string hash width

The ImageCache string hash now retains the full `strlen` width and folds any
high bits into the existing integer hash value. Long image names therefore do
not silently truncate their length before hashing, while ordinary names keep
the same compact cache-key behavior.

The focused regression is
`tests/automated/check-image-string-hash-width.py`.
