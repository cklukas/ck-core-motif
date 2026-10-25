# Clipboard format-atom allocation handling

`ClipboardGetAtomFromFormat` now rejects a null format name, checks the
synthetic atom-name size for overflow, and checks its allocation before
formatting. Any failure returns `None`, allowing callers to fail closed rather
than dereferencing an invalid temporary string.
