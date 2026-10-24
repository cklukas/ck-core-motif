# FontSelector numeric fields

`XmFontSelector` reads the numeric fields in an XLFD name into signed
`short` members.  The parser now uses a checked `strtol` conversion for point
size and both resolutions.  Empty fields and the XLFD `*` wildcard continue to
map to zero; malformed, out-of-range, or overflowing values also map to zero
instead of wrapping into a different font.

The size drop-down uses the same validation before converting a displayed
point size to Motif's tenths-of-a-point representation.  Values that cannot be
represented in the existing `short` field are treated as invalid, preserving
the public resource and widget ABI while preventing undefined selection and
font-list behavior.

`tests/automated/check-font-size-parsing.py` checks the conversion contract
and guards against reintroducing the unchecked `atoi` paths.
