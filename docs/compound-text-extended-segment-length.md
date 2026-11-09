---
nav_exclude: true
---
# Compound-text extended segment length

The compound-text extended-segment wire format encodes its payload length in
two seven-bit fields, so a single segment can represent at most 16,383 bytes.
When exporting a non-standard charset component, Motif now rejects a component
that cannot fit that encoding rather than wrapping the wire length and emitting
malformed compound text. Its temporary header allocation is also proved to fit
Xt's `Cardinal` size before allocation.

The focused regression is
`tests/automated/check-ctext-extended-segment-length.py`.
