# Bound drag target-list parsing to the serialized heap

Motif's byte-stream drag protocol stores target-list records before the
`heap_offset` declared in the property header.  The reader now validates each
record against that declared boundary, rather than using the complete X11
property length.  A malformed or extended property can no longer make the
parser consume bytes from the following heap or unrelated trailing data.

This is a wire-format hardening change: valid properties produced by Motif
retain the same layout and behavior.  The focused source regression is
`tests/automated/check-drag-target-heap-bound.py`.
