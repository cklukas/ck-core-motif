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
- Virtual-key translation rejects malformed keycodes before indexing the
  server keymap; the regression is qualified under a strict installed prefix.

## Open qualification gaps

These are real coverage gaps rather than claims of known source defects:

1. **x86_64 acceptance.** The native installed cohort is qualified on ARM64;
   x86_64 still needs a clean build and runtime pass covering Xft, ICU/HarfBuzz,
   AT-SPI, the pinned Xt, and independent C and C++ consumers.
2. **Strict sanitizer lifecycle.** The synthesized DropSMgr clipper leak is
   fixed and passes three repeated native runs plus a strict ASan/UBSan
   auto-drag lifetime fixture. The pinned Xt dependency patch also retires the
   destroyed-widget selection context; strict ABI, incremental-layout and
   clipboard-lifetime consumers pass with leak detection enabled. A strict
   FontSelector run still reports one 160-byte shared Xt/Xlib GC-cache
   allocation at process exit after Motif label teardown; the dependency
   boundary is documented in `docs/fontselector-sanitizer-boundary.md` and is
   not suppressed. Fontconfig, broader application ownership and other
   architectures remain open.
3. **External clipboard and DnD matrix.** The bounded Text/TextField fixture
   proves two-client INCR cancellation followed by a fresh transfer from the
   surviving owner when the pinned corrected Xt dependency is used. The
   installed transfer-storage cohort additionally passes target replacement,
   8/16/32-bit format assembly, merge bounds and repeated owner destruction
   against that dependency. DropSMgr auto-drag teardown now passes its own
   strict sanitizer fixture; direct Motif-to-Motif Text/TextField clipboard
   workflows now pass for mixed-script and multi-megabyte payloads. The new
   installed drop-site lifecycle probe covers registration,
   retrieval, update bracketing, explicit unregister/re-register and implicit
   registered-widget teardown. Drag-and-drop transfer cancellation, broader
   drag-and-drop ownership and unqualified system-library sanitizer ownership
   remain open; see `docs/dropsite-lifecycle.md`.
4. **Non-text Unicode matrix.** The installed public-API fixture now covers
   Label, List, ComboBox, Menu and FileSelection values for combining, non-BMP
   and Han text, plus a bounded two-segment compound `XmString` and List item
   search. Mnemonics, keyboard layouts and full application navigation still
   need the broader mixed-script and supplementary-plane journey coverage of
   Text and TextField.
5. **Accessibility user journeys.** AT-SPI protocol fixtures cover transport,
   cache identity, actions, selection, text boundaries, and multiple shells;
   the bounded native Orca journey now covers menus, lists, editable text,
   selection and disclosure/scrollbar state announcements. Complete desktop
   screen-reader, dialog/error-state and application journeys remain
   unqualified.
6. **Input-method services.** Local composition and selected IBus/m17n cases
   pass, but provider differences, candidate panels, keyboard layouts, and
   restart/cancellation behavior need a repeatable matrix.
7. **DPI and monitor changes.** Geometry defaults are covered; live mixed-scale
   monitor changes, pixmaps, clipping, hit testing, and explicit pixel sizes
   need measured 1x/2x/4x qualification.

## Product-level usability gaps

- Motif remains X11/Xft based. Xwayland is the supported Wayland deployment
  route; a native Wayland backend would require a separate backend design.
- The historical `XmThemes` option remains deliberately disabled; it is not
  the implementation used by this fork. The additive `XmAppearance` profiles
  (`classic`, `modern-light`, `modern-dark` and `high-contrast`) provide the
  qualified color, metric and focus defaults while preserving classic resource
  precedence. They are selected before widget creation and do not mutate
  realized widgets.
- The historical manual and visual corpus should be promoted selectively into
  deterministic Xvfb/native fixtures with loaded-library proof and receipts.
- The container layout code has already moved away from its old static tab-pool
  limitation; large-detail and unusual multi-display layouts still deserve a
  stress fixture.

## Recommended order

1. Qualify the current installed cohort on x86_64.
2. Extend the qualified clipboard cohort to drag-and-drop and application
   workflows without mixing it with unrelated held-storage changes.
3. Expand non-text Unicode and accessibility user-journey fixtures.
4. Extend strict ownership coverage to Fontconfig, drag-and-drop and full
   application workflows without weakening the dependency boundary.
5. Complete mixed-DPI qualification and expand the documented profile corpus.

Each item should land with a focused regression and a short entry in `CHANGES`.
