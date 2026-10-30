---
nav_exclude: true
---
# FontSelector bold and italic allocation

The bold/italic control builder now checks argument merging, its button box,
and both toggle buttons before adding callbacks or storing child pointers.
Partial construction returns cleanly and releases merged arguments.
