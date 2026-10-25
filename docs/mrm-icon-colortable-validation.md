# Mrm icon color-table validation

Icon conversion requires a serialized color table. `UrmCreatePixmap` now
rejects an icon whose color-table pointer is missing before color realization or
mapping, returning `MrmNOT_VALID` while preserving a cleared output. The
shared `Urm__RealizeColorTable` helper also rejects a null table so direct
internal callers cannot reach its first table-entry write with invalid data.

Valid icon conversion and color allocation are unchanged.
