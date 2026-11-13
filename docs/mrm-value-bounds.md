---
nav_exclude: true
---
# Mrm values must hold the bytes that are read from them

## Compound strings

A compound string in a UID file is the external byte stream of
`XmCvtByteStreamToXmString`: three ASN.1 header bytes, a length (one byte below
`0x80`, otherwise `0x82` and two bytes) and that many bytes of components. The
converter has no length parameter and reads as many bytes as the header
declares, so a stream whose header declares more than the literal or the widget
record holds was read past its buffer.

`Urm__CW_ValueFits` (which replaces `Urm__CW_UnitsValueFits`) now also checks a
`CString` value: the header and the declared length must lie inside the bytes
that the literal or the widget record holds from the start of the value. It is
applied in `Urm__FetchLiteral` (`MrmFetchLiteral`), in `Urm__CW_ReadLiteral`
(`MrmFetchSetValues`, widget argument lists, callback fixup) and, for values
inside a widget record, in the argument list and callback fixup. A stream that is
too short is a corrupt file: the fetch fails for that literal
(`MrmNOT_VALID` inside the library, `MrmFAILURE` from `MrmFetchLiteral`) and the
argument is not applied. Streams that fit are converted as before.

## Colours

A colour literal (`RGMColorDesc`) is either a name that ends in a NUL character
or three 16 bit components. `Urm__CW_ValueFits` checks that the name ends inside
the literal or the widget record and that an RGB colour holds all components,
before the colour is allocated. A malformed colour fails the fetch
(`MrmFetchColorLiteral`, `MrmFetchSetValues`, widget arguments) like the other
malformed literals.

## Test

```text
python3 tests/automated/run-mrm-value-bounds.py RESULTS
python3 tests/automated/run-mrm-value-bounds.py RESULTS2 --source-revision HEAD
```

`check-mrm-value-bounds.c` writes UID files with the resource manager's own
writer and reads them with `MrmFetchSetValues` and `MrmFetchLiteral` under
AddressSanitizer: colours with an unterminated name or too few components, and streams one byte short, half, or cut inside the header (short
and three byte length) fail cleanly, and exact-size streams convert. The crash
input is kept in `tests/fuzz/corpus/mrm-uid`.

The fuzz stand-in `XmCvtByteStreamToXmString` of `tests/fuzz/mrm-stubs.c` now
reads the length in the two forms of `lib/Xm/XmString.c`, and the class records
of the widget stand-ins carry their class name like real class records.
