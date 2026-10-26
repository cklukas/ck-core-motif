---
nav_exclude: true
---

# Clipboard snapshot formats

The Motif clipboard source path treats target lists and the `_MOTIF_SNAPSHOT`
identifier as 32-bit atom data.  It now requires that format explicitly: target
lists must be `XA_ATOM`/32-bit, and a snapshot must contain exactly one 32-bit
item before the value is cast to an atom.  Invalid conversion results are
discarded through the existing clipboard failure cleanup.

The regression check is
`tests/automated/check-clipboard-snapshot-format.py`.
