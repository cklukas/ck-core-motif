# XPM data pointer-row bounds

`XpmCreateDataFromXpmImage` allocates one pointer row for each header, image,
and extension line.  The individual dimensions are bounded before allocation,
but their combined count must also fit in the unsigned row index used to place
the serialized string area.  The serializer now rejects a combined-row
overflow before allocation and pointer arithmetic.

This preserves the existing XPM format for valid images while returning
`XpmNoMemory` for an unrepresentable layout.  The focused source-contract check
is:

```sh
python3 tests/automated/check-xpm-data-row-count-bounds.py
```
