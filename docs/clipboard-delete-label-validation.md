# Clipboard delete-label validation

`ClipboardDeleteItemLabel` now validates the data-item lookup status and
minimum record length before reading the label ID. Missing or malformed items
are released and ignored instead of entering fatal corruption handling.
