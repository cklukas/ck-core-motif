---
nav_exclude: true
---
# Maintenance review and suggested work

The first widget review was a bounded starting point. The age of the inherited
code is a reason to continue targeted maintenance; neither a short patch list
nor a large test count establishes that the toolkit is finished.

## Concrete storage and image corrections

The next source review identified the following independently useful fixes.
Their regression procedure and compatibility contracts are documented under
[clipboard storage and image input](storage-and-images.md).

| Finding | Effect | Correction |
| --- | --- | --- |
| Clipboard property allocation combines native first-chunk bytes with wire bytes remaining | Multi-request format-32 reads overrun storage on LP64 | Assemble checked native-size chunks; retain wire offsets separately |
| Conversion merges narrow unsigned item counts to signed int byte counts | Overflow can underallocate the output or read an enormous source length | Bound the complete result to Xt's allocation limit before arithmetic |
| Merge reallocates before copying a source alias | The appended source can refer to freed storage | Keep the old value alive until both copies complete |
| Unified-transfer destruction uses call data as the selection atom | It disowns atom zero rather than the owned selection | Read the closure argument and retain one callback per selection |
| XPM comment parsing does not stop at EOF/NUL | Malformed image input can hang or read past its buffer | Bound comment and string/token scans at the actual input end |
| XPM memory character reads advance beyond NUL and treat high bytes as signed | Truncated pixel tokens overrun; valid high-byte symbols can fail | Return EOF at NUL and read bytes unsigned |
| XPM unsigned decimal fields wrap | Oversized dimensions can be accepted as smaller values | Reject overflow before multiplying and adding |
| Buffer-to-file writing ignores final flush failure and rejects empty output | Success can leave incomplete output; an empty buffer fails unnecessarily | Check byte writes and close; accept zero bytes |
| File-to-buffer read count uses int and a size failure closes only its descriptor | Large-file comparison truncates; stream storage leaks | Retain size_t counts and close the owning FILE |

## Continuing review priorities

These are proposed work packages with explicit acceptance criteria, not claims
that all listed behavior is broken. Independent reproducers should decide each
change. Preserve the public resource/callback model; document intentional
behavior changes, and version any unavoidable binary layout changes.

| Priority | Area | Useful work and acceptance evidence |
| --- | --- | --- |
| High | Large-text editing and layout | Measure loading, edits and redraw separately at 1 MiB and larger sizes. Compare incremental results with fresh layout, including CRLF, bidi, wrapping and font changes. Legacy line-table reflow and TextField modify-verify snapshots must validate their allocations and retain safe state after failure. The current incremental-layout work has its own source and native qualification. |
| High | Unicode across the whole widget set | Extend real input, selection, search and mnemonic tests to List, ComboBox, labels, menus, file dialogs and compound strings. Cover combining sequences, non-BMP characters, mixed scripts and invalid locale input; text-editor shaping alone does not establish these widgets. |
| High | Selection and drag/drop lifecycle | Exercise external owners, cancellation, owner replacement, reentrant callbacks, destruction and property changes, with leak checks. Include 8/16/32-bit formats and request boundaries. Invalid null-widget and `None`-atom ownership requests must fail before Xt state access. Keep dependency defects distinct from toolkit defects. |
| High | Geometry at large fonts and display scales | Audit Dimension/Position/int conversions, overflow, clipping and hit tests across scrollable widgets and very long labels. Test real widget windows at 1x/2x/4x and explicit application dimensions. List and zero-based ComboBox APIs that expose one-based `int` positions must reject values whose compatibility offset is not representable. |
| High | Widget lifetime and resource ownership | Audit every timeout/work procedure, registered callback, borrowed resource and cached display object. Callback arrays and Xlib-reported window lists must be checked against both `size_t` and Xt `Cardinal` limits. Repeated create/change/destroy cycles and callback-driven destruction must pass instrumented native tests. |
| Medium | DPI and monitor changes | Define an opt-in live scaling policy and test font/control metrics, geometry negotiation and monitor changes together. Explicit application sizes must remain predictable; changing all legacy pixel resources implicitly would be disruptive. |
| Medium | Image usability | Consider scale-aware image lookup and optional modern image/vector decoding behind clear ownership/cache APIs. Test alpha, masks, symbolic colors and cache invalidation at several sizes before adding formats. Existing XPM safety comes first. |
| Medium | Keyboard and focus | Review traversal, popup dismissal, modality, menu accelerators, repeat behavior and visible focus using native keyboard/pointer dispatch and destruction during callbacks. |
| Medium | Accessibility completeness | Continue native list/menu/table semantics and precise editing notifications with a real external observer and assistive client. Provider availability alone is insufficient. |
| Medium | Portability and maintenance tooling | Qualify x86_64 as well as ARM64, optional feature-off builds, current compiler diagnostics, installed C/C++ consumers, deterministic generators and malformed resource/image inputs. |

A general replacement of X11 geometry, a complete Unicode conformance claim or
an incompatible default scaling policy needs a separate design and evidence.
The immediate maintenance path is concrete fixes plus reusable tests, followed
by the next reproducible issue in these work packages.

## TreeView typography and selection

TreeView labels now use the active X font's ascent, descent, and measured width
for their baseline and RTL placement.  This keeps text readable when an
application supplies a larger or smaller font and avoids the former fixed
eight-pixel width estimate.  `XmTreeViewSetSelectedId` also scrolls a visible
selected node into view, which makes programmatic navigation behave like the
existing keyboard path.  The focused source check is:

```sh
python3 tests/automated/check-treeview-text-metrics.py
```

DataGrid headers and cells use the same font-metric approach for their
baselines; fixed sixteen-pixel baselines previously clipped text in short rows
and left larger fonts visually off-centre.  The focused check is:

```sh
python3 tests/automated/check-datagrid-text-metrics.py
```

Calendar headings, weekday labels, and day numbers now use measured font
baselines as well.  The focused check is:

```sh
python3 tests/automated/check-calendar-text-metrics.py
```

NumberBox and Stepper value text use the same measured-baseline rule, keeping
compact numeric controls aligned with adjacent labels at different font sizes.
The focused check is:

```sh
python3 tests/automated/check-number-stepper-text-metrics.py
```
