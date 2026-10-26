# Text SetValues wide-string conversion

Runtime text `SetValues` handling for `XmNwcValue` now bounds the conversion
buffer and checks allocation and `wcstombs` results before updating the source.
Failed conversion leaves the current source value intact while consuming the
invalid resource value safely.

The focused source contract is checked by
`tests/automated/check-text-setvalues-wcs.py`.
