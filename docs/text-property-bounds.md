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
