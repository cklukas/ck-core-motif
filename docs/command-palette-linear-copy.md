---
nav_exclude: true
---
# CommandPalette display-label assembly

Command-palette filtering already computes the exact label-plus-shortcut size.
It now fills that buffer with offset-based copies instead of repeated string
scans, preserving the displayed text while keeping rebuild work linear for
large command lists.

The source contract is checked by:

```sh
python3 tests/automated/check-command-palette-linear-copy.py
```
