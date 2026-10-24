# Text property conversion bounds

`XmCvtXmStringTableToTextProperty` now rejects negative counts and non-empty
tables without a table pointer. Its compound-string conversion writes the final
terminator within the allocated buffer instead of advancing past the allocation.
The source contract is covered by
`tests/automated/check-text-property-bounds.py`.

Compound-text conversion also checks the pointer-array multiplication and each
string’s contribution to the signed `nitems` total before allocating the output.
Overflow cleans up the partially converted entries and returns `BadAlloc`; the
aggregate sizing contract is covered by
`tests/automated/check-text-property-size.py`.

The compound-string (ASN.1) branch applies the same checked aggregate sizing,
allocates space for its final terminator, and verifies each conversion result
before copying it. The branch contract is covered by
`tests/automated/check-compound-string-bounds.py`.

Locale, text, and string conversion applies the same pointer-array and signed
aggregate checks, including cleanup of all temporary converted strings when the
total cannot be represented. The branch contract is covered by
`tests/automated/check-locale-text-property-size.py`.

The reverse conversion validates output pointers, property lengths, and required
value storage before reading an X property. Compound-text element counting also
rejects `int` counter overflow and pointer-table allocation overflow. The input
contract is covered by `tests/automated/check-text-property-inputs.py`.

Compound-text import now finds segment boundaries with `memchr` limited to the
declared property length, copies each segment into a terminated temporary string,
and retains the historical trailing empty element. Missing terminators therefore
cannot make the converter read beyond Xlib’s payload. The parser contract is
covered by `tests/automated/check-compound-text-import.py`.

Motif compound-string (ASN.1) import now verifies every encoded element length
against the property’s remaining bytes before advancing or converting it. Zero
or truncated element lengths return `XConverterNotFound` rather than looping or
reading outside the Xlib payload. The contract is covered by
`tests/automated/check-compound-string-import.py`.

An empty Motif compound-string property now returns an empty table without
dereferencing a null payload. The empty-property contract is covered by
`tests/automated/check-compound-string-empty.py`.

Locale, STRING, and UTF-8 import handles an empty property as one empty
`XmString` and uses non-underflowing bounds for non-empty payloads. Oversized
property lengths are rejected before the `int` element counter is used. The
contract is covered by `tests/automated/check-text-property-empty.py`.
