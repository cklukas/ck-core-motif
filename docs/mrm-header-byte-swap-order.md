# Mrm header byte-swap validation order

Header data-entry fields are stored in the file byte order. The header readers
now swap a foreign-endian entry header before validating its `validation` and
`entry_size` fields. This keeps valid byte-swapped files readable and ensures
capacity checks operate on the native size value.
