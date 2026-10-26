---
nav_exclude: true
---
# DnD message subtype bounds

Motif drag-and-drop client messages carry a compact subtype in the event
payload. The decoder now rejects values outside the internal callback table,
and the shared subtype mapper returns the private invalid-reason sentinel
instead of indexing past the table. This keeps malformed external-owner
traffic from reaching callback decoding with an out-of-bounds read.

The focused regression is
`tests/automated/check-drag-message-type-bounds.py`.
