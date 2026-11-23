---
nav_exclude: true
---
# Bugfix Notes Archive 14

## `xmstring-draw-length-bounds.md`

# Compound String draw-length bounds

Compound String entries retain their byte count as an unsigned value, but the
Xlib, Xmb, UTF-8, and Xft draw interfaces accept signed `int` lengths.  Before
preparing an RTL buffer or calling a drawing interface, `_XmStringDrawSegment`
now rejects a segment larger than `INT_MAX`.  This prevents a negative
converted draw length and prevents the RTL loop's signed counters from
overflowing.

Normal labels and compound strings are unchanged.  An unrepresentably large
single segment is skipped safely rather than reaching a drawing backend with an
invalid length.

The focused contract check is:

    python3 tests/automated/check-xmstring-draw-length-bounds.py

## `xmstring-entry-allocation.md`

# XmString entry allocation failures

Internal unoptimization and segment insertion now allocate array entries with
an explicit failure check before initializing them. Failed entry allocation
leaves the source string and any copied segment ownership intact instead of
dereferencing a missing entry or leaking temporary storage.

## `xmstring-entry-copy-input.md`

# XmString entry-copy input validation

`_XmStringEntryCopy`, `EntryCvtToOpt`, and `EntryCvtToUnopt` reject malformed
unoptimized entries that advertise a positive text length while carrying no
text pointer. This prevents a `memcpy` through `NULL` when corrupted or
partially constructed internal data reaches a copy or representation-conversion
operation. `_XmStringOptToNonOpt` retains its null-string and destination
allocation guards; optimized string text is inline storage, so it has no
nullable text-pointer state to validate. The conversion reports failure when
its destination segment cannot be allocated.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-entry-copy-input.py
```

## `xmstring-entry-create-allocation.md`

# XmString entry creation allocation

The internal `_XmEntryCreate` macro now checks each `XtMalloc` result before
zeroing or initializing the record.  Allocation failure therefore produces a
null entry that callers can unwind, instead of dereferencing a null pointer.
Successful entry initialization is unchanged.

## `xmstring-enumeration-locking.md`

# XmString enumeration locking

`XmStringIsVoid` and `XmCvtXmStringToByteStream` now delegate synchronization
to `XmeStringGetComponent` rather than holding the process lock across repeated
component reads. This prevents recursive locking while preserving serialized
context access.

## `xmstring-func-warning-guards.md`

# XmStringFunc warning-backed guards

`XmParseMappingGetValues` receives an Xt `ArgList`, whose `value` field is an
integer `XtArgVal`, not a pointer. Comparing it with `NULL` produced a
portable compiler warning; zero is the correct empty-value check.

`MakeStr` also had an unbraced null-result check that made the next component
fetch appear guarded when it was not. The check is now braced and the fetch is
explicitly sequenced after successful concatenation. Public parse and string
APIs are unchanged.

## `xmstring-function-locking.md`

# XmString helper locking

`XmStringHasSubstring` and `XmeGetNextCharacter` now delegate synchronization
to the string helpers they call instead of holding the process lock across
those calls. This avoids recursive locking during substring scans and parse
callback component creation.

## `xmstring-generate-locking.md`

# XmStringGenerate locking

`XmStringGenerate` now delegates synchronization to parse, cache, component,
and concatenation helpers instead of holding the process lock across those
calls. Rendition wrapping and fallback behavior remain unchanged.

## `xmstring-generate-rendition-failure.md`

# XmStringGenerate rendition failure

When `XmStringGenerate` is asked to apply a rendition, it may convert compact
segments and grow rendition-tag arrays.  Allocation or size-check failures in
those steps now release the generated string and return `NULL`.  The function
no longer reports an unwrapped string as successful output when the requested
rendition could not be applied.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-generate-rendition-failure.py

## `xmstring-generate-table-failure.md`

# XmString generation table failure

`XmStringGenerate` and `_XmStringUngenerate` depend on a shared parse table
for tab and newline mappings.  If that table cannot be allocated or
initialized, both APIs now return `NULL` rather than continuing with an empty
table and silently changing the conversion semantics.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-generate-table-failure.py

## `xmstring-get-output-validation.md`

# XmString extraction output validation

`XmStringGetNextComponent` now rejects missing output addresses before
advancing a context. `XmStringGetLtoR` likewise rejects a missing text output
pointer before initialization, returning failure without dereferencing it.

## `xmstring-getcomponent-locking.md`

# XmStringGetNextComponent locking

`XmStringGetNextComponent` now delegates synchronization to
`XmeStringGetComponent` instead of holding the process lock across that call.
This avoids recursive locking while keeping component extraction serialized by
the existing helper.

## `xmstring-getltr-locking.md`

# XmStringGetLtoR locking and initialization

`XmStringGetLtoR` now delegates synchronization to the public context helpers
it calls instead of holding the process lock across them. It also checks
`XmStringInitContext` before enumeration, so context allocation failure returns
`False` cleanly rather than entering iteration with a null context.

## `xmstring-length-bounds.md`

# XmString length bounds

`XmStringLength()` has a public signed-`int` return type, while its internal
byte-stream conversion reports an unsigned length.  The helper now clamps
unrepresentable lengths to `INT_MAX` before returning, preventing overflow for
large compound strings and giving List, ComboBox, and SelectionBox callbacks a
stable ABI value.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-xmstring-length-bounds.py
```

## `xmstring-lock-boundaries.md`

# XmString helper lock boundaries

`XmStringTableToXmString` delegates to string-copy and concatenation APIs that
manage the Motif process lock themselves; it no longer wraps those calls in a
second lock. Parse-table cleanup likewise delegates mapping and string frees
without holding an outer lock. This prevents recursive-lock deadlocks while
preserving the existing ownership behavior.

The table parse and unparse helpers follow the same rule: their per-string
conversion APIs own the process lock, so the table loops no longer add an
outer lock.

Parse-mapping value access follows the same rule when copying a substitute
string: the mapping helper does not hold an outer process lock around
`XmStringCopy`.

## `xmstring-make-str-concat.md`

# XmString context slice concatenation

`MakeStr` now checks every component allocation and concatenation while
building a context slice.  Any failure frees the accumulated slice and
returns null instead of silently returning partial text.

## `xmstring-make-str-failure.md`

# XmString context slicing failures

`MakeStr` now checks whether `_XmStringSegmentNew` actually inserted each
copied segment.  If allocation fails and the new string remains incomplete,
the context is marked erroneous, the partial string is freed, and null is
returned.

## `xmstring-make-str-segment.md`

# XmString context segment slicing

`MakeStr` now checks each `MakeStrFromSeg` result before concatenating it.
Failed segment slices free the accumulated output and return null instead of
being treated as an optional missing operand.

## `xmstring-mb-allocation.md`

# XmString multibyte conversion allocation

`_XmGetMBStringFromXmString` now checks the result buffer in both its
component and segment conversion paths. A failed allocation releases the
active component data and conversion context before returning `NULL`; normal
conversion output is unchanged.

`tests/automated/check-xmstring-mb-allocation.py` protects this contract.

## `xmstring-nconcat-bounds.md`

# XmStringNConcat copy-limit bounds

`XmStringNConcat()` derives the `XmStringNCopy()` limit by adding the first
string length and the requested byte count.  The addition is now checked in
the signed `int` API domain; negative or unrepresentable limits return `NULL`
instead of wrapping into a smaller copy.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-xmstring-nconcat-bounds.py
```

## `xmstring-ncopy-conversion-failure.md`

# XmStringNCopy conversion failure

`XmStringNCopy` first serializes the source string to Motif's ASN.1 form so
it can honor the requested byte limit.  A failed serialization reports zero
length and no buffer; that state is now rejected before the no-truncation
shortcut.  Applications therefore receive `NULL` on allocation/serialization
failure instead of a misleading successful copy.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-ncopy-conversion-failure.py

## `xmstring-obsolete-locking.md`

# Legacy XmString constructor locking

`XmStringLtoRCreate` now delegates synchronization to the direction,
concatenation, and separator constructors it calls. The compatibility API no
longer holds the process lock across those nested operations.

## `xmstring-obsolete-locks.md`

# Legacy XmString compatibility lock boundaries

The compatibility APIs `XmStringNConcat`, `XmStringNCopy`, and
`XmStringByteCompare` delegate to modern conversion, copy, and concatenation
functions that manage the Motif process lock themselves. Their redundant outer
locks were removed to prevent recursive-lock deadlocks; ownership and return
values remain unchanged.

## `xmstring-optimized-rendition-allocation.md`

# Optimized XmString rendition allocation

The optimized-source branch of `XmStringConcatAndFree` now checks its
single-tag rendition allocation before storing the tag.  Allocation failure
releases the owned input strings and returns `NULL`, matching the checked
multi-tag branch and preventing a null-pointer write.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-optimized-rendition-allocation.py
```

## `xmstring-parse-array-inputs.md`

# XmString parse-array inputs

`XmStringTableParseStringArray` now rejects a null source element before
calling the parser.  Already-created strings are released and the temporary
table is freed, preserving the wrapper's existing all-or-nothing failure
behavior.

## `xmstring-parse-initial-allocation.md`

# XmStringParseText initial allocation

`XmStringParseText` now checks creation of the initial tag component before
processing patterns and unmatched text.  A failed allocation releases the
process lock and returns null rather than continuing with a partial result.

## `xmstring-parse-mapping-inputs.md`

# XmString parse-mapping inputs

`XmParseMappingSetValues` now rejects a null argument array when the requested
count is nonzero and skips entries with null resource names. Valid mappings and
zero-count calls retain their existing behavior, while malformed input leaves
the mapping unchanged for that entry.

## `xmstring-parse-unmatched-failure.md`

# XmStringParseText unmatched-component failure

`parse_unmatched` now reports component creation or concatenation failure
instead of silently leaving the input pointer unchanged.  `XmStringParseText`
unwinds the partial result and releases its process lock at each call site,
preventing allocation-failure loops and partial output.

## `xmstring-pattern-conversion.md`

# XmString pattern conversion

When a parse pattern and input use different text representations,
`match_pattern` converts the wide character through `wctomb`.  Conversion
failure is now treated as a non-match in both directions, so an unrepresentable
character cannot make the matcher inspect an uninitialized temporary buffer.

## `xmstring-put-rendition-copy.md`

# XmStringPutRendition source-copy failure

`XmStringPutRendition` now checks the source-string copy before concatenating
the opening rendition marker.  A failed copy releases that marker and returns
null instead of producing a string containing only rendition boundaries.

## `xmstring-put-rendition-failure.md`

# XmStringPutRendition allocation failures

`XmStringPutRendition` now treats failure to create either rendition-boundary
component as an operation failure.  It no longer returns a partially modified
or unadorned copy, and releases the intermediate string when the closing
component cannot be allocated.

## `xmstring-realloc-failure.md`

# XmString concat realloc failure

`XmStringConcatAndFree` now checks all reallocations used while merging
compatible segments.  The optimized segment and ordinary text paths preserve
the original allocation on failure, release the owned input strings, and
return `NULL` before later metadata or text writes can dereference an invalid
pointer.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-realloc-failure.py
```

## `xmstring-rendering-warnings.md`

# XmString rendering warning paths

The optimized-to-nonoptimized conversion and optimized line renderer use
stack-local optimized segment records. Generic entry setters also support a
larger unoptimized record, so using those setters on the stack-local record
caused compiler out-of-bounds diagnostics. The optimized fields are now
assigned directly.

The entry-copy size branches and allocation check now explicitly guard the
allocation, and
rendition-tag cleanup only reallocates an initialized size. When all tags are
removed, the old tag array is released explicitly. These changes preserve the
existing XmString and rendition APIs while making ownership and bounds
contracts explicit.

The optimized concatenation path now tests the text type rather than the
address of its inline text array, which is never null. This retains the
intended tab-compatibility decision and removes a misleading always-true
diagnostic.

## `xmstring-rendition-allocation.md`

# XmString rendition-tag allocation

`XmStringConcatAndFree` now validates rendition-end tag counts before
allocating the copied tag array and checks the allocation result before
copying tags.  On failure it releases the owned input strings and returns
`NULL`, matching the surrounding concat allocation paths instead of
dereferencing a null array.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-rendition-allocation.py
```

## `xmstring-rendition-bounds.md`

# XmString rendition-tag bounds

Segment parsing checks the rendition-tag count before allocating copied or
borrowed tag arrays, including Xt's `Cardinal` byte-size range. If a malformed
context reports an unrepresentable count, parsing stops cleanly with no tag
array returned. Accepted tag-array byte counts are passed explicitly to Xt;
ordinary rendition ordering and ownership are unchanged.

The source-level regression is
`tests/automated/check-xmstring-rendition-bounds.py`.

## `xmstring-rendition-resize.md`

# XmString rendition resize failures

Rendition begin/end setters now resize their tag arrays transactionally. The
existing pointer and count remain unchanged when `XtRealloc` fails, avoiding a
null dereference and preserving the entry's prior formatting state.

## `xmstring-rendition-tag-bounds.md`

# XmString rendition-tag bounds

The XmString segment extractor copies rendition tags through an Xt allocation.
Both extraction paths now reject a tag count whose byte size exceeds either
`size_t` or Xt's `Cardinal` range before invoking the copy helper.  This keeps
the public extraction behavior unchanged while preventing a narrowed size on
large or malformed compound strings.

## `xmstring-segment-allocation.md`

# XmString segment allocation

`_XmStringSegmentNew` now checks entry-array and segment-array allocation before
publishing mutations, including Xt's `Cardinal` byte limit for segment growth.
Copy failures and allocation failures leave the existing XmString structure
usable, while successful segment insertion retains the existing layout and
ownership behavior.

The focused regression check is
`tests/automated/check-xmstring-segment-allocation.py`.

## `xmstring-segment-array-allocation.md`

# XmString segment-array allocation

When concatenation combines implicit lines, `XmStringConcatAndFree` grows or
creates a segment-pointer array.  The combined segment count is now checked
before multiplication, and both realloc/malloc results are checked before the
array is written.  Failure releases the owned inputs and returns `NULL`
without leaving a partially installed segment array.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-segment-array-allocation.py
```

## `xmstring-segment-copy-allocation.md`

# XmString segment-copy allocation guards

The non-mutating `XmStringConcat` path temporarily copies an existing segment
array when a newline representation must be moved into a cloned string. The
copy now checks the segment count against both `size_t` and Xt `Cardinal`
limits, and handles allocation failure by restoring the original string state
before returning `NULL`.

The focused source-contract check is
`tests/automated/check-xmstring-segment-copy-allocation.py`.

## `xmstring-segment-copy-bounds.md`

# XmString segment copy bounds

When a compound-string segment is copied, its character count is now checked
before terminator and padding space and the Xt allocation width are computed.  Invalid counts fail through the existing tag/text cleanup
path instead of wrapping the allocation size.

The focused source contract is checked by
`tests/automated/check-xmstring-segment-copy-bounds.py`.

## `xmstring-segment-copy-cleanup.md`

# XmString segment-copy cleanup

`_XmStringGetSegment` asks the component iterator for owned copies before
normalizing a segment.  If a second allocation fails or a count is invalid,
the function now frees those intermediate tag and text copies before clearing
the outputs.  This prevents leaks during wide-character conversion and other
copy-data failure paths.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-segment-copy-cleanup.py

## `xmstring-segment-failure-cleanup.md`

# XmString segment failure cleanup

`_XmStringGetSegment` can obtain owned component copies before discovering an
invalid legacy count or failing rendition-tag allocation.  Failed extraction
now releases the owned rendition tags and the copies made so far before
returning `FALSE`, and leaves all output pointers in a deterministic empty
state.  The text and the tag are borrowed from the string and its tag cache
until they are copied, so they are never released on a failure; see
[xmstring-stream-bounds.md](#xmstring-stream-boundsmd).

The focused source-contract check is:

    python3 tests/automated/check-xmstring-segment-failure-cleanup.py

## `xmstring-segment-line-growth.md`

# XmString segment line growth

When adding a segment beyond the current line range, XmString grows its line
entry array through Xt.  The growth check now covers both `size_t` and Xt's
`Cardinal` byte range before resizing, while retaining the existing cleanup on
allocation failure.

## `xmstring-segment-merge-bounds.md`

# XmString segment merge bounds

`XmStringConcatAndFree` only merges adjacent segments when the resulting byte
count fits the destination representation.  Unoptimized segments are guarded
against unsigned-integer wraparound, while optimized segments retain their
8-bit byte-count limit; oversized text remains in separate compatible segments
instead of truncating the stored length.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-segment-merge-bounds.py
```

## `xmstring-segment-validation.md`

# XmString segment validation

`_XmStringGetSegment` now validates the context and every required output
pointer before initializing outputs. A null context can no longer be
dereferenced while preparing the compatibility direction value.

## `xmstring-single-segment-outputs.md`

# XmString single-segment output validation

`_XmStringSingleSegment` now validates both output pointers before clearing or
writing them. Invalid callers receive `False` without dereferencing a null
output address.

## `xmstring-stream-bounds.md`

# Compound string byte stream: component bounds, tag cache, segment reader

`XmCvtByteStreamToXmString` has no length parameter. It trusts the length in the
stream's own header, so every component it reads must lie inside that length.

## Components are validated one by one

The converter first scans the components to decide whether the string can be
stored in the optimized single-segment form. That scan stops at the first
component that cannot be optimized, for example a separator. The builder for
the general form, `_XmStringNonOptCreate`, then walked the remaining components
without a bounds check, so a component with a length that exceeds the stream was
read past its end. Both passes now use one function, `_check_component`, which
rejects a component when

- fewer bytes than a tag and a length field remain (the long form needs two
  more length bytes),
- its value is longer than the bytes that remain in the stream, or
- it is a direction or layout push component without its one byte value.

A stream with such a component is malformed and `XmCvtByteStreamToXmString`
returns `NULL`, as it does for the other malformed streams. Valid streams,
optimized or not, convert as before.

## The tag cache compares counted tags exactly

`_XmStringIndexCacheTag` looks a tag up in the process-wide tag cache. A tag
that the caller passes with a length is the first `length` bytes and ends at its
first NUL character, like every tag. The comparison used `strncmp` and then read
the cached tag at index `length`; a cached tag shorter than the incoming tag,
whose first bytes agreed up to an embedded NUL, was read past its own end. The
function now ends a counted tag at its first NUL character, and a counted tag
equals a cached tag only when the cached tag has exactly that length and the
bytes are equal.

## XmStringGetNextSegment releases only what it allocated

`_XmStringGetSegment` returns the tag and the text of a segment as pointers into
the string (or its context), and copies them for the caller only when it has
found a segment. The failure paths freed the pointers that were not yet copied:
the tag cache entry, a constant default tag, or the text of the string. Now
nothing is released on a failure except the copies made by the function itself
(the copied rendition tags, and the tag and the text copy when a later copy
fails), `*text` and `*tag` are `NULL`, and the string and the tag cache keep
what they own.

The text copy had the same kind of fault for wide character text. The length
of a text component is a number of bytes for every text type, but the copy
multiplied it by the size of `wchar_t` for wide character text and read four
times the data of the segment. The copy now takes the byte count, pads it to a
whole number of wide characters and ends it with a null wide character.

## Test

```text
python3 tests/automated/run-xmstring-stream-bounds.py RESULTS
python3 tests/automated/run-xmstring-stream-bounds.py RESULTS2 --source-revision HEAD
```

`check-xmstring-stream-bounds.c` is compiled with the production `lib/Xm`
sources under AddressSanitizer and UndefinedBehaviorSanitizer, with the stand-ins
of `tests/fuzz` for the display; no toolkit build tree and no X server are
needed. Each stream is held in a heap block of exactly the length that its header
declares. With `--source-revision` the sources of that Git revision are tested
and the runner succeeds only if every defect case fails there. The crash inputs
that found the defects are kept as seeds in `tests/fuzz/corpus/xmstring-stream`.

## `xmstring-substring-null-tags.md`

# XmString substring matching with null tags

Substring highlighting can encounter a segment without an explicit rendition
tag.  Matching now normalizes both the searched segment and the underlined
segment to `XmFONTLIST_DEFAULT_TAG` before comparing or checking the current
charset, avoiding null-string operations while preserving default-tag
matching.

## `xmstring-table-break-concat.md`

# XmString table break concatenation

`XmStringTableToXmString` now checks the concatenation that adds each break
component.  A failed allocation returns null immediately instead of allowing a
later iteration to continue from a missing accumulated result.

## `xmstring-table-break-copy.md`

# XmString table break-component copying

When a non-null break component is supplied, `XmStringTableToXmString` now
checks its copy before concatenation.  A failed copy releases the accumulated
result and returns null; a null break component remains the documented
separator-free option.

## `xmstring-table-concat-failure.md`

# XmString table concatenation failure propagation

`XmStringTableToXmString` now stops immediately when concatenating a valid
table element fails.  It no longer lets the subsequent break-component step
mask that failure with a partial result.

## `xmstring-table-entry-copy.md`

# XmString table entry copying

`XmStringTableToXmString` now checks each table-entry copy before
concatenation.  A failed copy releases the accumulated result and returns
null, rather than being interpreted as an optional missing operand.

## `xmstring-table-inputs.md`

# XmString table concatenation inputs

`XmStringTableToXmString` now rejects a null table element rather than
silently omitting it from the concatenated result.  The accumulated string is
freed before returning null, making malformed input an explicit all-or-nothing
failure.

## `xmstring-table-locking.md`

# XmString table conversion locking

`XmStringToXmStringTable` now delegates synchronization to its context and
string helpers instead of holding the process lock across them. Its table
allocation failure path also frees the active context before returning.

## `xmstring-table-size-failure.md`

# XmString-table destination-size failure

`CvtStringToXmStringTable` now frees every generated `XmString` and its table
when the destination buffer is too small. The converter still reports the
required pointer size and returns failure, but no longer leaks the successful
intermediate allocation on that error path.

`tests/automated/check-xmstring-table-size-failure.py` protects this contract.

## `xmstring-table-transaction.md`

# XmString table conversion transactions

`XmStringTableParseStringArray` and `XmStringTableUnparse` now treat the
result table transactionally. If an element conversion fails, previously
created XmStrings or output buffers are freed along with the table and the
functions return `NULL`; successful conversions are unchanged.

## `xmstring-tablist-locking.md`

# XmString tab-list proposal locking

`XmStringTableProposeTablist` now delegates synchronization to tab-list,
conversion, and rendering helpers instead of holding the process lock across
them. Cleanup behavior on allocation and invalid-input failures is unchanged.

## `xmstring-tag-allocation.md`

# XmString tag-cache allocation

`_XmStringIndexCacheTag` now checks allocation of a new cached tag before
copying its bytes. Failure releases the Motif process lock and returns the
existing cache-failure index instead of dereferencing a null buffer.

`tests/automated/check-xmstring-tag-allocation.py` protects this contract.

## `xmstring-truncate-header-bounds.md`

# XmString ASN.1 truncation header bounds

`_XmStringTruncateASN1` now verifies that the top-level header and each
remaining component contain complete tag/length headers before reading encoded
lengths. Both short and long ASN.1 length forms are checked, so truncated
external compound text is rejected without reading past the supplied buffer.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-truncate-header-bounds.py
```

## `xmstring-unoptimize-cleanup.md`

# XmString unoptimization cleanup

`Unoptimize` now releases its newly allocated array-entry object when segment
size validation fails before segment storage can be allocated.  This closes an
early-return leak without changing successful conversion behavior.

## `xmstring-unoptimize-failure.md`

# XmString optimized-entry conversion failures

Concatenation can convert optimized entries to their general representation.
Those conversions allocate memory and may fail. The conversion helper now
retains the original entry when allocation fails, and every concatenation path
checks for a `NULL` result before dereferencing or storing it. The public
operation releases its owned operands and returns `NULL` instead of crashing or
publishing a malformed string.

The focused source-contract check is:

```text
python3 tests/automated/check-xmstring-unoptimize-failure.py
```

## `xmstring-unparse-array-inputs.md`

# XmString unparse-array inputs

`XmStringTableUnparse` now rejects a null source element before converting it.
Any prior converted values and the temporary output array are released, so
callers receive an all-or-nothing result instead of a partial table.

## `xmstring-unparse-failure.md`

# XmStringUnparse failure propagation

`XmStringUnparse` now reports allocation, overflow, and character-conversion
failures from its text helper.  The helper marks the output length invalid;
the public API frees any partial result and returns `NULL` instead of silently
returning truncated or incomplete text.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-unparse-failure.py

## `xmstring-unparse-multibyte-length.md`

# XmString unparse multibyte lengths

The unparse path now checks the `mblen()` result for a multibyte parse pattern
before passing it to the component renderer.  Invalid sequences are omitted
from that substitution instead of converting `-1` into an oversized unsigned
length.

## `xmstring-wide-conversion-bounds.md`

# Wide XmString segment conversion bounds

`XmStringGetNextSegment` converts wide-character segments to the current
locale.  The old allocation divided the maximum multibyte capacity by
`sizeof(wchar_t)`, which could allocate too little space (or zero bytes) for
short segments and silently truncate the result.

The conversion now allocates `char_count * MB_CUR_MAX` bytes with checked
`size_t` and Xt `Cardinal` arithmetic, and rejects results that cannot fit the
API's `short` character-count output. Existing representable segments keep
their exact text and terminator. The source regression in
`tests/automated/check-xmstring-wide-conversion-bounds.py` protects the
capacity and range checks.

## `xpm-buffer-color-output-bounds.md`

# XPM buffer color output bounds

The buffer-export serializer uses a fixed temporary line for each XPM color.
`WriteColors` now checks the remaining capacity and the `snprintf` result before
advancing the line pointer. A truncated symbolic-color description returns
`XpmNoMemory` without reading or forming a pointer beyond that buffer.

The focused source regression is
`tests/automated/check-xpm-buffer-colors-bounds.py`.

## `xpm-buffer-extension-output-bounds.md`

# XPM buffer extension output bounds

The XPM buffer exporter now treats extension serialization as a checked
operation. `WriteExtensions` validates extension names and line arrays, checks
each `snprintf` result against the remaining output capacity, and reports
failure to the public exporter instead of allowing an unsigned size underflow
or a truncated pointer advance.

The focused source regression is
`tests/automated/check-xpm-buffer-extensions-bounds.py`.

## `xpm-buffer-pixel-index-bounds.md`

# XPM buffer pixel-index bounds

XPM buffer serialization now passes the image color count into its pixel
writer. Each pixel index is validated before indexing the color table, and
remaining-buffer arithmetic is checked before every row write. Malformed image
data therefore returns `XpmFileInvalid` instead of reading past the color
table or wrapping the output span.

The focused source contract is checked with:

```sh
python3 tests/automated/check-xpm-buffer-pixel-index-bounds.py
```

## `xpm-closeness-comparator.md`

# Xpm color closeness ordering

Color candidate sorting now compares `long` closeness values relationally
instead of subtracting and narrowing the result to `int`. Extreme color
distances therefore retain a valid `qsort` ordering without overflow.

## `xpm-color-output-bounds.md`

# XPM color output bounds

The XPM image serializer builds each color description in a fixed temporary
buffer. `CreateColors` now checks the remaining capacity and the return value
from `snprintf` before advancing its output pointer. A truncated description
returns `XpmNoMemory` without forming an out-of-bounds pointer.

The focused source regression is
`tests/automated/check-xpm-create-colors-bounds.py`.

## `xpm-comment-marker-lengths.md`

# XPM comment marker length checks

The in-memory XPM comment parser uses the configured beginning and ending
comment markers with `strncmp` and pointer advances. Their lengths now remain
`size_t`, matching `strlen` and the C library interfaces, instead of narrowing
before those operations. The loop index used to copy the ending marker uses the
same width. This preserves behavior for ordinary XPM data while avoiding
truncation for unusually large marker strings.

`tests/automated/check-xpm-comment-marker-lengths.py` keeps the source contract
under regression coverage.

## `xpm-comment-size-bounds.md`

# XPM comment-size bounds

XPM buffer serialization now accumulates hint, color, and pixel comments in a
`size_t` temporary with checked additions. Null metadata and values that cannot
fit the public `int` size contract fail before output allocation; the caller
propagates that failure instead of using a wrapped comment size.

The focused source contract is checked with:

```sh
python3 tests/automated/check-xpm-comment-size-bounds.py
```

## `xpm-create-image-size.md`

# XPM image-data allocation size

`CreateXImage` now checks the XImage row stride against `SIZE_MAX` and performs
the row-count multiplication in `size_t` before allocating image data. This
avoids an intermediate signed-integer overflow while retaining all valid image
dimensions.

The focused regression is `tests/automated/check-xpm-create-image-size.py`.

## `xpm-data-pixel-index-bounds.md`

# XPM image pixel-index validation

`XpmCreateDataFromXpmImage` now validates every pixel index before looking up
the corresponding color string while serializing an `XpmImage`.  It also
checks the `width * cpp + 1` row-size arithmetic and reserves room for each
row terminator before writing.

Malformed images with an out-of-range pixel index, a missing color string, or
an overflowing row size return `XpmFileInvalid` instead of indexing beyond the
color table or writing past the generated data buffer.  Valid images retain
the existing serialized format and API behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-xpm-data-pixel-index-bounds.py
```

## `xpm-data-row-count-bounds.md`

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

## `xpm-extension-size-bounds.md`

# XPM extension-size bounds

XPM buffer serialization now checks extension names and lines before summing
their lengths. Null extension storage and `unsigned int` addition overflow
return a failure before the output buffer is allocated; the caller propagates
that failure instead of using a wrapped size. The writer also tracks remaining
capacity for each extension line and the final `XPMENDEXT` terminator, so a
cursor inconsistency cannot turn an exact allocation into an out-of-bounds
copy.

The focused source contract is checked with:

```sh
python3 tests/automated/check-xpm-extension-size-bounds.py
python3 tests/automated/check-xpm-data-extension-output-bounds.py
```

## `xpm-file-pixel-index-bounds.md`

# XPM file pixel-index validation

`XpmWriteFileFromXpmImage` now validates each image pixel index against the
image color table before copying its serialized color key.  A missing color
string is rejected as well.  Invalid images return `XpmFileInvalid` without
writing an out-of-bounds color-table entry; valid XPM output is unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-xpm-file-pixel-index-bounds.py
```

## `xpm-grayscale-color-bounds.md`

# XPM grayscale color replacement bounds

`_XmConvertToBW` can receive XPM colors whose original strings are shorter
than the 13-character expanded grayscale form. It now allocates a replacement
string, formats it with a checked `snprintf`, and swaps it only after the new
value is ready. It first requires the original value to be exactly the
expanded 12-digit hex form, so short or malformed `#` values are not read past
their terminator. Allocation failure leaves the original color unchanged. This
avoids both over-reading and writing past short color-string allocations while
preserving the existing grayscale conversion.

The focused source contract is checked with:

```sh
python3 tests/automated/check-color-bw-replacement-bounds.py
```

## `xpm-hash-free-null.md`

# Xpm hash cleanup null handling

`xpmHashTableFree` now returns immediately for a null table pointer before
reading its fields. Error-unwind paths can therefore call cleanup safely even
when table initialization did not complete.

## `xpm-hash-growth-bounds.md`

# XPM hash-table growth bounds

The XPM color-key hash table doubles its slot count when it reaches its load
limit.  Growth now rejects a zero or already-maximal slot count before the
doubling macro runs, preventing unsigned wraparound from turning an oversized
table into a small allocation.  The existing post-growth allocation-size
check remains in place.

Valid tables retain the same growth policy; an unrepresentable growth returns
`XpmNoMemory`.

The focused source-contract check is:

```sh
python3 tests/automated/check-xpm-hash-growth-bounds.py
```

## `xpm-hash-growth-rollback.md`

# Xpm hash growth rollback

When an Xpm hash table reaches its load threshold, insertion now rolls back
the newly allocated atom if table growth fails. The failure still reports
`XpmNoMemory`, but the table no longer retains an uncounted entry or leaks the
atom allocation.

## `xpm-hash-probe-wrap.md`

# Xpm hash probe wrapping

Hash-table collision probing now wraps when the current bucket is the first
bucket before decrementing. The previous order formed a pointer one element
before the allocation, which is undefined behavior even when it was not
dereferenced.

## `xpm-scan-growth-bounds.md`

# XPM pixel-map growth bounds

The XPM image scanner grows its distinct-pixel table as new colors are found.
The growth path now checks both the slot-count doubling and the resulting
`sizeof(Pixel)` allocation size before calling `XpmRealloc`.  If either value
cannot be represented, scanning fails cleanly instead of wrapping the size and
writing beyond the resized table.

Normal images keep the existing doubling behavior.  The focused source-contract
check is:

```sh
python3 tests/automated/check-xpm-scan-growth-bounds.py
```

## `xpm-strcasecmp-byte-safety.md`

# Xpm fallback case-insensitive comparison

The optional fallback `xpmstrcasecmp` now converts input bytes to
`unsigned char` before calling `tolower` and compares the terminating bytes
without signed-char arithmetic. Non-ASCII byte values therefore no longer
trigger undefined behavior or platform-dependent ordering.

## `xpm-string-duplicate-input.md`

# XPM fallback string duplication input

When XPM is built on a platform without `strdup`, its local `xpmstrdup`
implementation now rejects a null source pointer before measuring it and checks
the terminating-byte increment for size overflow. This keeps the fallback
helper consistent with the library's other input-validation paths.

The focused source regression is
`tests/automated/check-xpm-string-duplicate-input.py`.
