# Clipboard delete-formats validation

`ClipboardDeleteFormats` now validates the data-item record, format-list
offset/count bounds, and every retrieved format record before iterating or
reading fields. Malformed records are released and ignored without fatal
corruption handling or out-of-bounds list traversal.
