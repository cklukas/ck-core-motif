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

## Incremental immutable source maps

`XmTextUnicodeMapRetain` acquires an independent reference; each owned reference
needs one `XmTextUnicodeMapFree`. Keep all reference operations on the owning
thread. Maps copy their UTF-8 input and survive their original buffer or native
widget. `XmTextUnicodeMapReplace(map, first_scalar, last_scalar, utf8, bytes)`
returns a new owned map for the half-open replacement. It consumes neither input.
Invalid input, allocation failure or reference/size overflow returns NULL and
preserves every existing snapshot.

Unchanged LF-delimited paragraph bodies share their byte/scalar/grapheme indexes.
A replacement reanalyzes whole affected hard paragraphs, including its end
paragraph. CRLF stays together. This relies on the hard-break rules in
[Unicode grapheme segmentation](https://www.unicode.org/reports/tr29/), rather
than an arbitrary character margin. A long single paragraph can still require
full analysis. The flat paragraph-prefix table costs O(paragraph count), and
lookups find their paragraph by binary search.

`XmTextUnicodeMapGetUsage` reports the construction's `mapped_bytes`,
`mapped_scalars`, `rebuilt_paragraphs` and `reused_paragraphs`. These measure
analysis work, not retained memory or subsequent lookup cost. The 1 MiB,
8,192-paragraph regression replaces one scalar while mapping 130 bytes and 129
scalars, rebuilding one paragraph and sharing 8,191. A separate 65 KiB single
paragraph deliberately allows full-context analysis.

Native TextField retains its map across cursor operations; native shared Text
sources publish one updated map for all their views. Approved replacement deltas
are staged and published with committed bytes/length before redraw callbacks.
Rejected edits preserve the old map. Whole-value assignments invalidate captures;
appearance-only TextField SetValues retains its map. A verifier may inspect a
proposal and veto it: the restored value must receive fresh map/layout captures.
Wide-character SetValues passes native characters and scalar counts to its
verifier and installs its final approved replacement, including an empty value.

`XmTextUnicodeLocaleAvailable` distinguishes native UTF-8 position semantics
from explicit UTF-8 map creation, which remains locale-independent. Custom
sources and non-UTF-8 locales keep native scalar scanning. A failed required
UTF-8 map refuses the interactive step with a warning instead of splitting a
grapheme through scalar fallback. Later requests can retry construction.

The immutable map and allocation-failure fixtures compare against full map
reconstruction and an independent whole-input ICU break iterator. Native widgets
exercise real public cursor actions, shared sources, callback acceptance/rejection
and retained snapshots. This establishes incremental source indexing, not
incremental shaping or complete live-editor projection reuse. Glyphs, wrapping,
bidi-context changes, selection geometry, external input methods and full
application acceptance remain separate gates. See the [ABI contract](abi.md):
the new TextField record requires a coherent epoch 8 rebuild.

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

## Bounded font and shaping contexts

`XmTextFontContextCreate` creates an explicit display/screen/primary-font
resolver. Native text widgets retain one context across paragraphs and edits.
Hosts may use `XmTextParagraphCreateWithContext` and
`XmTextLayoutCreateWithContext` for the same ownership model. The default hard
ceilings are 256 cached grapheme keys, 256 KiB of UTF-8 key storage and 64 retained
font/shaping leases; explicit limits may lower them, including zero. Oversized
keys are uncached. Positive and confirmed-negative whole-grapheme resolutions
are bounded independently of active captures.

Captures hold independent font leases. Eviction, context invalidation and
context destruction cannot invalidate a retained capture. Each shaping call
reactivates the exact Xft/FreeType font size before using its retained HarfBuzz
font; reusing a different size on the same font face does not alter old captures.
Cache allocations and temporary failures are not treated as confirmed missing
font coverage. `XmTextFontContextGetUsage` reports saturating counters.

Font/render-table changes require a new context. Current Fontconfig object
replacement is detected, but in-place Fontconfig/Xft-default changes require
`XmTextFontConfigurationChanged`; widgets lazily recreate their context after
that process-wide notification. Hosts serialize actual configuration mutation
against display operations. The notification itself is thread-safe; it does not
make Fontconfig mutation concurrent. Custom hosts may explicitly invalidate a
context. The borrowed primary font and display must outlive their contexts and
captures.

The exact 1,073,152-byte mixed-script fixture retains byte/scalar/grapheme,
caret and edit invariants. On the measured native ARM64 system, bounded resolver
and shaping reuse reduced wrapped Text set from 51.557 s to 2.309 s and TextField
mutation from 6.703 s to 2.344 s. A long single-line edit still rebuilds its complete
capture; these measurements do not establish instantaneous editing or a general
latency guarantee. Tests force independent key/count/font eviction, disabled
caches, font/config replacement and retained-capture painting. Aggregate installed
cohort verification remains separate from these focused candidate results.

## Native widgets and composition

### Local Compose and dead-key regression

`tests/Toolkit/Im/local_compose.c` and its Python observer specify a local XIM
regression independently of external input-method services. The observer owns a
fresh authenticated X server and window manager, explicitly selects `@im=none`,
and supplies the standalone `local_compose.Compose` input. Native XIM and both
widget XIC registrations must exist; the raw `XLookupString` fallback is not
accepted as proof of composition. The private display must advertise no external
XIM server.

Both shell-sharing and per-widget policies use real XTest key events. Compose
acute and dead acute commit U+00E9; a separate Compose rule commits U+0065 U+0301
as one native value change. Pending prefixes must leave committed bytes and
callback counts intact. Public widget snapshots check exact UTF-8, scalar caret
positions and focus isolation, followed by native whole-grapheme Left/Right and
BackSpace. Explicit public table/font resources and native display owners are
released during teardown. Original, test and restored private keymaps, exact
Compose bytes, source/binary and loaded library identities remain in the receipt.

Native AArch64 qualification passes both policies with 76 observations and
80 injected keys, preserving the original private keymap. This finite fixture does
not test a system Compose corpus, external IBus/m17n engines, candidate panels,
physical keyboard layouts, clipboard transport or an assumed Escape contract.

### Native captures and preedit

Text/TextField cache and invalidate captures when source text or relevant
resources change. Wrapped lines share immutable paragraph analysis. Paint,
measurement, hit testing, selections and visual movement use the same capture;
a failed enabled layout reports failure rather than substituting raw unshaped
source drawing.

XIM preedit replacement spans, length limits and multibyte conversions are
validated before updating the composition buffer. A deletion may retain the
server's previous advisory caret; the bridge clamps that hint to the resulting
buffer instead of dropping a valid deletion. Feedback-only updates preserve
characters. Widget callbacks receive an owned, terminated character slice.

Input acceptance uses bounded locale-character decoding and character properties,
independently of glyph coverage or advance width. Combining marks and format
characters therefore remain valid even when they have no advance. Text's preedit
and reset paths support Xft as well as fontsets; scalar counts are never reused
as UTF-8 byte capacities.

A provider can deliver committed bytes before its final preedit deletion or done
notification. `XmImMbLookupString` finishes transient widget composition before
returning those committed bytes. Late draw/done notifications cannot delete the
commit or reset its caret. `XmImMbResetIC` similarly clears composition when the
IC uses initial reset state; a provider-returned commit and a cancellation are
distinct outcomes. Preserve-state contexts retain their reset semantics.

Preedit is composition state, not an application undo journal. The real-service
fixture verifies that preedit/cancellation does not emit committed-value changes
and that a Telex commit emits one native value-change notification. Applications
retaining engine-owned undo must route committed edits to that owner and project
the accepted revision back to the widget, keeping one domain history.

### Service and handle lifetime

The toolkit observes XIM destruction and invalidates every associated transport
handle before any further lookup, reset or focus call. Active preedit is cancelled
without modifying committed contents. Registered widgets and owned copies of the
toolkit-defined IM resources survive transport loss; an XIM availability callback
opens the new service, rebuilds toolkit-owned ICs and restores their focus. The
same path handles a provider that starts after the widgets already exist. Both
`XmPER_SHELL` and `XmPER_WIDGET` policies use the checked callback bridge.

`XIM`/`XIC` pointers are transport-scoped: do not cache them across event-loop
dispatch. `XmImSetXIC(widget, NULL)` queries the currently registered live IC.
Opaque application-created ICs and custom XIM attributes cannot be reconstructed
by the toolkit; their owner must resupply them after loss. Resource copies retain
callback functions and their borrowed application closures; the application keeps
those closures alive until unregistering the widget. `XmImCloseXIM` explicitly
closes the current registration and availability observation. These operations
belong to the owning display thread.

### Real-provider fixture and limits

The opt-in fixture runs through an actual IBus XIM provider on an owned Xvfb
server and D-Bus session, with fresh XDG directories and an explicit unique bus
endpoint. It starts no desktop panel and changes no user desktop settings. Key
input uses XTest; focus transfer uses native traversal. It never manufactures
preedit callbacks as evidence of provider operation.

```sh
python3 tests/automated/check-native-features.py \
  --prefix /absolute/installed/prefix \
  --out /absolute/fresh/validation-directory \
  --only xim-service
```

The recorded focused ARM64 Linux run used IBus **1.5.32-2**, ibus-m17n
**1.4.36-1**, libm17n **1.8.5-1**, m17n-db **1.8.10-1** and libX11
**2:1.8.12-1**, in `C.UTF-8` with Xft. Both sharing policies passed late service
startup, non-ASCII Vietnamese Telex preedit/commit/cancellation, active-composition
service loss and same-process reconnection. Hindi ITRANS supplied a two-scalar
single grapheme; the Unicode engine supplied CJK and supplementary emoji scalars.
The test checked native scalar positions, whole-grapheme motion/deletion and
committed-value callbacks. Focus/reset can return a provider commit or
cancellation; the fixture records that outcome and requires matching exact
contents, caret and notification counts, with no surviving ghost preedit.

This is focused source-linked validation; complete installed-cohort verification
remains a separate gate. The tested Unicode-engine/XIM path supplied no committed
lookup bytes for standalone U+0301. That case remains a provider/transport
limitation, not a successful combining-mark service test; the Hindi grapheme and
logical acceptance checks do not remove that qualification. Other engines,
providers, candidate-panel interaction and broad input-method interoperability
remain unqualified. Consult the [validation limits](status.md) before claiming
complete input-method, clipboard, shaping or rich-text support.


## Default composition presentation

The vendor shell now prefers `OnTheSpot,OverTheSpot,OffTheSpot,Root` for native
XIM preedit. Applications retain explicit resource overrides and the remaining
styles stay available when the service cannot provide inline callbacks. The
previous order selected root/server presentation before inline composition,
which can leave pending input invisible when an isolated service has no panel.
The real IBus fixture now exercises the default rather than overriding it with
an application resource. Dedicated real-service runs exercise both shell and
widget input policies with that default. Successful runs cover preedit, commit,
cancellation and service restart, but one repeat misses a Hindi preedit update;
repeatable service qualification remains open. Actual-application revalidation
is pending; committed input observations alone do not prove preedit ownership.
