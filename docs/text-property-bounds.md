---
nav_exclude: true
---
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
rejects totals outside Xt's `Cardinal` byte range, passes an explicit
Xt-sized allocation, allocates space for its final terminator, and verifies
each conversion result before copying it. The branch contract is covered by
`tests/automated/check-compound-string-bounds.py`.

A zero-length serialization result denotes failure for a non-null compound
string, including when its byte-stream allocation fails. The property converter
returns `BadAlloc` and clears/frees its partial value; it no longer reports
success with missing serialized elements and an overstated `nitems`. Missing
payloads and results exceeding the remaining allocation are also rejected.
Serializer lengths retain their unsigned type until checked against the signed
aggregate limit. Successful ASN.1 output and the public API remain unchanged.

Locale, text, and string conversion applies the same pointer-array and signed
aggregate checks, passes explicit Xt `Cardinal` byte sizes for its pointer and
final buffers, and includes cleanup of all temporary converted strings when the
total cannot be represented. The branch contract is covered by
`tests/automated/check-locale-text-property-size.py`.

Locale/string conversion releases its temporary pointer array when any item
conversion fails, including before strict-mode compound-text fallback. Aggregate
sizing occurs after all items have been converted, so overflow releases every
item, including those after the item that exceeded the size limit. Successful
output bytes, encoding choices and fallback behavior remain unchanged.

The executable ownership regression extracts the production property import/export
functions and exercises 69 cases across the locale/string and ASN.1 styles: every
allocation failure, per-item conversion errors, oversized aggregate lengths,
strict fallback, serializer failures and successful output bytes. Allocation, length
and conversion test doubles allow overflow and rollback checks without large
buffers or an X server. Outstanding allocations and application-lock balance
must return to zero after failures; success retains only the caller's result.

```sh
python3 tests/automated/run-text-property-ownership-unit.py --out /absolute/fresh/results
```

The runner uses strict compiler warnings and ASan/UBSan and records the tested
source hash, commands and results in `receipt.json`. This unit scope does not
qualify the locale provider's actual character conversion or X11 transfers.

ASN.1 import rollback destroys each completed `XmString` with `XmStringFree`,
including after a later decoder failure or post-decode length rejection. A raw
`XtFree` only releases the outer record and leaks nested compound-string
storage. The executable test models separately owned records and payloads,
injects failures at all seven allocations of a three-element import, rejects
each decoder position and late length check, and verifies successful ownership
transfer and cleanup. Valid imported strings and public APIs are unchanged.

An empty compound-text property still imports as one empty `XmString`. If
creating that string fails, the converter frees its temporary table and returns
`XConverterNotFound`, matching failed element conversion in the non-empty path.
It no longer returns `Success` with a null table element. The unit test covers
failure at each of the table/string/payload allocations and successful cleanup
of the valid empty result.

The reverse conversion validates output pointers, property lengths, and required
value storage before reading an X property. Compound-text element counting also
rejects `int` counter overflow and pointer-table allocation overflow. The input
contract is covered by `tests/automated/check-text-property-inputs.py`.

Compound-text import now finds segment boundaries with `memchr` limited to the
declared property length, checks Xt-sized table and segment allocations, copies
each segment into a terminated temporary string, and retains the historical
trailing empty element. Missing terminators therefore cannot make the converter
read beyond Xlib’s payload. The parser contract is covered by
`tests/automated/check-compound-text-import.py`.

Motif compound-string (ASN.1) import now verifies every encoded element length
against the property’s remaining bytes before advancing or converting it. Zero
or truncated element lengths return `XConverterNotFound` rather than looping or
reading outside the Xlib payload. The contract is covered by
`tests/automated/check-compound-string-import.py`.

An empty Motif compound-string property now returns an empty table without
dereferencing a null payload. The empty-property contract is covered by
`tests/automated/check-compound-string-empty.py`.

ASN.1 header decoding also verifies the short or long length header is fully
present before calling the legacy length helper, preventing malformed short
payloads from being read past their X property buffer. The contract is covered
by `tests/automated/check-compound-string-header.py`.

Locale, STRING, and UTF-8 import handles an empty property as one empty
`XmString` and uses non-underflowing bounds for non-empty payloads. Oversized
property lengths are rejected before the `int` element counter is used. The
contract is covered by `tests/automated/check-text-property-empty.py`.
