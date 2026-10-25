# Drag atom-table property validation

`ReadAtomsTable` reads the `_MOTIF_DRAG_ATOMS` cache property as an 8-bit
serialized record.  It now requires the expected property type and format,
then checks that the advertised atom count fits completely within the bytes
returned by Xlib before indexing any entries.  Malformed cache data is
discarded while the existing table remains usable.
