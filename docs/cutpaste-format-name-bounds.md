---
nav_exclude: true
---
# Clipboard format atom-name bounds

Clipboard format atoms are assembled from a fixed Motif prefix and an
application-provided format name. The allocation length is checked before
allocation, and the resulting string is written with bounded formatting.

The source contract is covered by
`tests/automated/check-cutpaste-format-name-bounds.py`.
