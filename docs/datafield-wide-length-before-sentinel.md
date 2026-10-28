# DataField wide-string length before sentinel reads

`df_FindPixelLength` temporarily terminates a wide string at the requested
length before converting it for measurement.  The sentinel read must happen
only after the length is known to be nonnegative and representable; otherwise
a malformed negative or oversized length can index outside the caller’s
buffer.  Both font-set paths now validate first, then save and replace the
sentinel as before.

The regression contract is checked by
`tests/automated/check-datafield-wide-length-before-sentinel.py`.
