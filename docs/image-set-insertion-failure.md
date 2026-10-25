---
nav_exclude: true
---
# Built-in image-cache insertion failure

`InitializeImageSet` now treats a failed hash-bucket insertion as initialization
failure. It does not publish a partially populated built-in image table; the
existing cleanup path releases all allocated entries and the private hash
table, allowing a later retry to start from a clean state.

The focused regression is
`tests/automated/check-image-set-insertion-failure.py`.
