# Clipboard format-length validation

`ClipboardGetLenFromFormat` now rejects failed or malformed format properties
before reading their payload. The synthetic format atom must exist, and the
property must be a non-empty X11 format-32 result; otherwise the helper
returns `ClipboardFail` with the existing default length of 8 bits.
