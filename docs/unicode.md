# Unicode text and input

Build with `--enable-xft --enable-unicode-text` to use the shared ICU/HarfBuzz
layout pipeline in native Text and TextField widgets. A UTF-8 locale is required
for the scalar position contract below; set the locale and Xt language procedure
before creating widgets. Missing dependencies are configure errors, rather than
silently enabling an incomplete layout path.

## Coordinate systems

| Coordinate | Meaning |
| --- | --- |
| UTF-8 byte offset | Exact encoded source offset; an interior encoding byte is invalid. |
| Widget/scalar position | Unicode scalar index in a UTF-8 locale, not a UTF-8 byte count. |
| Grapheme boundary | ICU UAX #29 extended-grapheme boundary; legal interactive caret/deletion step. |
| Visual position | Pixel coordinate plus caret affinity in a captured shaped line. |

`<Xm/TextUnicode.h>` exposes an immutable `XmTextUnicodeMap`. Create it with
`XmTextUnicodeMapCreate(utf8, byte_length)` and release it with
`XmTextUnicodeMapFree`. The map owns its metadata. Scalar/byte conversions are
exact; they do not round an interior offset. Previous/next-grapheme operations
are explicit. Malformed UTF-8, embedded NUL, invalid scalar encodings and input
beyond the ICU `INT32_MAX` byte limit are refused.

A combining sequence, flag pair or emoji ZWJ sequence can contain several
scalars but form one interactive step. Programmatic source scans still count
scalars. Changing their count unit globally would corrupt geometry and callers
that correctly expect scalar positions.

## Paragraphs and line captures

`<Xm/TextLayout.h>` provides `XmTextParagraphCreate`,
`XmTextParagraphLine` and `XmTextParagraphWrapLine`. A paragraph owns a copy of
its text, full ICU bidi/isolate analysis and selected fallback fonts. The primary
`XftFont` is borrowed and must outlive the paragraph and all its line captures. Each line
capture retains its paragraph. Release captures with `XmTextLayoutFree` and
paragraph references with `XmTextParagraphFree`.

ICU derives visual runs from the complete paragraph and its actual line range.
HarfBuzz receives the complete actual line as context across font/script items;
letters outside the line cannot continue Arabic joining past a break. Fontconfig
fallback requires coverage of the complete grapheme. A resolved primary font's
file or PostScript identity is not reused as a fallback query.

Wrapping uses ICU UAX #14 opportunities and shared paragraph advance indexes,
then verifies and adjusts actual line-edge shaping. An unbroken word may use
extended-grapheme emergency breaks. A single grapheme wider than the viewport
is retained to guarantee progress. Newlines belong to the host paragraph/line
composer; do not pass embedded CR/LF to `XmTextParagraphCreate`.

`XmTextLayoutCaret`, `Hit`, `VisualMove`, `SelectionRegion` and `Draw` consume
one immutable capture. Affinity 0 is the leading edge of the following grapheme;
affinity 1 is the trailing edge of the preceding grapheme. They can differ at
bidi boundaries. When a font provides no GDEF ligature carets, insertion points
are evenly distributed inside that ligature. Selection clips are computed from
the captured visual regions, including mixed-direction logical ranges.

All paragraph/capture operations, font access and destruction run on the
owning display thread. The primary `XftFont` must outlive every paragraph and
capture using it. These objects are not a worker-thread text service.

## Native widgets and composition

Text/TextField cache and invalidate captures when source text or relevant
resources change. Wrapped lines share immutable paragraph analysis. Paint,
measurement, hit testing, selections and visual movement use the same capture;
a failed enabled layout reports failure rather than substituting raw unshaped
source drawing.

XIM preedit changes are checked for negative/out-of-range replacement spans,
length overflow, invalid multibyte conversion and invalid caret positions before
buffer replacement. Preedit is composition state, not an application undo
journal. Applications retaining engine-owned undo must route committed edits to
that owner and project the accepted revision back to the widget; they should not
create a second independent domain history in an adapter.

Bounds checks and synthetic fixtures do not prove real XIM service behavior.
Consult the [validation limits](status.md) before claiming input-method,
clipboard, complete shaping or rich-text support.
