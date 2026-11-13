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
