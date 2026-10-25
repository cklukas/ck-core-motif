# Motif 360 review

This review covers the current `ck-core-motif` tree, its installed native ARM64
fixtures, and the build/tooling paths used by embedders. It is intentionally
split between fixes already landed, regressions now covered, and work that
still needs a separate environment or a larger end-to-end fixture.

## Completed source fixes

The following high-impact defects are fixed and have focused regressions:

- UIL listing diagnostics no longer write through an unbounded `vsprintf` path.
- Image scaling checks dimensions, strides, coordinate ranges, and allocation
  sizes before converting to Motif geometry types.
- Text source conversion rejects ranges that cannot be represented safely.
- WML and UIL parser generation works in parallel and from an out-of-tree build.
- Standard target construction rejects unrepresentable target counts.
- External selection ownership is covered through claim, replacement, widget
  destruction, and selection cleanup.
- Multi-megabyte mixed-script UTF-8 clipboard round trips are covered with a
  chunk boundary inside a multibyte sequence.
- The list accessibility model keeps stable child identities while native rows
  are reordered or removed.

## Open qualification gaps

These are real coverage gaps rather than claims of known source defects:

1. **x86_64 acceptance.** The native installed cohort is qualified on ARM64;
   x86_64 still needs a clean build and runtime pass covering Xft, ICU/HarfBuzz,
   AT-SPI, the pinned Xt, and independent C and C++ consumers.
2. **Strict sanitizer lifecycle.** The existing leak report is at the Xt
   selection-context boundary. It needs an ownership-level fix or a narrowly
   scoped native lifecycle test before it can be called clean.
3. **External clipboard and DnD matrix.** The bounded Text/TextField fixture
   now proves two-client INCR cancellation followed by a fresh transfer from
   the surviving owner when the pinned corrected Xt dependency is used. The
   broader matrix still needs target replacement, 8/16/32-bit formats,
   drag-and-drop cancellation, callback destruction, and leak checks.
4. **Non-text Unicode matrix.** Lists, menus, labels, combo boxes, file
   selection, mnemonics, search, and compound strings need the same mixed-script,
   combining-mark, supplementary-plane, and keyboard-navigation coverage as
   Text and TextField.
5. **Accessibility user journeys.** AT-SPI protocol fixtures cover transport,
   cache identity, actions, selection, text boundaries, and multiple shells.
   Repeatable Orca journeys for menus, lists, editable text, dialogs, selection,
   and error states remain unqualified.
6. **Input-method services.** Local composition and selected IBus/m17n cases
   pass, but provider differences, candidate panels, keyboard layouts, and
   restart/cancellation behavior need a repeatable matrix.
7. **DPI and monitor changes.** Geometry defaults are covered; live mixed-scale
   monitor changes, pixmaps, clipping, hit testing, and explicit pixel sizes
   need measured 1x/2x/4x qualification.

## Product-level usability gaps

- Motif remains X11/Xft based. Xwayland is the supported Wayland deployment
  route; a native Wayland backend would require a separate backend design.
- The theme option is deliberately disabled because its implementation is not
  present. A bounded color/metric/focus/hover API can improve integration while
  preserving classic resource defaults.
- The historical manual and visual corpus should be promoted selectively into
  deterministic Xvfb/native fixtures with loaded-library proof and receipts.
- The container layout code has already moved away from its old static tab-pool
  limitation; large-detail and unusual multi-display layouts still deserve a
  stress fixture.

## Recommended order

1. Qualify the current installed cohort on x86_64.
2. Finish the two-client clipboard/DnD lifecycle matrix without mixing it with
   unrelated held storage changes.
3. Expand non-text Unicode and accessibility user-journey fixtures.
4. Resolve the Xt selection-context sanitizer ownership issue.
5. Add opt-in theme metrics and complete mixed-DPI qualification.

Each item should land with a focused regression and a short entry in `CHANGES`.
