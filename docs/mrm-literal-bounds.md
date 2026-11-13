# UID files: units literals and data entry offsets

## Units literals must be as large as their record

A UID literal of type `HorizontalInteger`, `VerticalInteger`, `HorizontalFloat` or
`VerticalFloat` is a record (`RGMUnitsInteger`, `RGMUnitsFloat`) that
`Urm__CW_ConvertValue` reads as a whole. The UIL compiler writes it with the
size of the record, but the size in a file is data like any other: a shorter
literal made the converter read its value and units beyond the buffer that holds
the literal.

The size of the literal is checked against the size of the record before it is
fixed up or converted, in `Urm__FetchLiteral` (`MrmFetchLiteral`) and in
`Urm__CW_ReadLiteral` (`MrmFetchSetValues`, the widget argument lists and the
callback fixup), and a value with units that is stored inside a widget record is
checked against the end of that record. A literal that is too short is a corrupt
file: the fetch fails (`MrmNOT_VALID` inside the library, `MrmFAILURE` from
`MrmFetchLiteral`) for that literal, like the other malformed literals, and the
argument is not applied.

## Data entry offsets must be aligned

The data entries of a data record and of the header record are read in place
through a structure with 4 byte alignment. The writer places every entry at a
full-word offset of the record data, so the offset in a handle is always a
multiple of four. An offset from a file that is not aligned is now rejected
as a corrupt file by `Idb__DB_GetDataEntry`, `Idb__HDR_GetDataEntry`, and the
two filter matchers, and the writers refuse to append behind a damaged free
pointer of that kind. Before, the read was an access to a misaligned structure,
which is undefined behaviour in C even where the hardware tolerates it.

## Test

```text
python3 tests/automated/run-mrm-literal-bounds.py RESULTS
python3 tests/automated/run-mrm-literal-bounds.py RESULTS2 --source-revision HEAD
```

`check-mrm-literal-bounds.c` writes UID files with the resource manager's own
writer and reads them back with the production `lib/Mrm` sources under
AddressSanitizer and UndefinedBehaviorSanitizer (the display and the widget
library are the stand-ins of `tests/fuzz`). Valid float and integer literals,
and aligned entries of data and header records, still load. The crash inputs that
found the defects are kept as seeds in `tests/fuzz/corpus/mrm-uid`.
