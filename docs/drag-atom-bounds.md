---
nav_exclude: true
---
# Drag atom table bounds

Motif drag-and-drop state is shared through properties on the per-display Motif
window. Property counts are untrusted X server data, so table-size arithmetic
is checked before allocation and before serializing a property. This prevents a
malformed or corrupted count from wrapping an allocation size. The source
contract is covered by `tests/automated/check-drag-atom-bounds.py`.

Generated `_MOTIF_ATOM_` names are formatted with a bounded operation into
the fixed atom-name buffer, keeping the property-table path safe even when
the entry index is supplied by a large or corrupted table.

The per-display `XContext` IDs are initialized and tested as integer IDs,
avoiding pointer-to-integer diagnostics on 64-bit builds.

The drag-icon context uses the same integer-zero convention, so related
per-display state also compiles without pointer-to-integer diagnostics.
