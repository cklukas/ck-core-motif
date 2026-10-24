# Drag atom table bounds

Motif drag-and-drop state is shared through properties on the per-display Motif
window. Property counts are untrusted X server data, so table-size arithmetic
is checked before allocation and before serializing a property. This prevents a
malformed or corrupted count from wrapping an allocation size. The source
contract is covered by `tests/automated/check-drag-atom-bounds.py`.
