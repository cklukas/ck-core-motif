# Modern appearance integration qualification

This cohort integrates toolkit-owned modern styling, natural control geometry,
connected Notebook tabs, symmetric rounded drawing, refined scrollbars, SVG
dialog glyphs, and the gallery and Periodic demonstrations. ColorPicker provides
RGB, HSV, Lab D50 and pencil modes with shared alpha, preview, apply and revert.
Classic rendering remains available. Native consumers must rebuild for ABI 9.

The integration preserves the maintenance branch’s bounds, allocation and
callback-lifetime fixes. Native compilation also corrected invalid error returns
in UIL, missing conversion declarations, helper declaration order and duplicated
merge helpers. Redesigned controls retain teardown guards and owned label cleanup.

The full-gallery acceptance test exposed a text gap-buffer regression: after
capacity growth, replacement used the pre-growth gap size when deleting the old
range. Subsequent shorter values could leave the previous text visible. Deletion
now expands the relocated gap by the removed range. A dedicated native test
alternates empty, short and long values through growth and compaction; it
reproduced the failure before the correction.

Acceptance also covers focused-descendant Sidebar shortcuts, mandatory TabBox
scroll-arrow creation, and Column label teardown. Column constraint callbacks
and getters validate the parent Column rather than the child field. TabStack
selection initializes its child iterator after assigning the validated parent.
Scaled XYBitmap images retain their one-bit depth so XPutImage applies the GC
foreground/background mapping when drawing into a color pixmap. A native pixel
regression reproduced the previous failure and covers four scale factors.

Button focus uses concentric rounded inner and outer boundaries, maintaining
stroke thickness through all corners while preserving the button face. Native
pixel checks compare every corner at multiple scales. Variable ScrolledWindow
layout remains valid without an internal clip child, including MainWindow;
Periodic now verifies that its overview fills the available work width.

## Validation

Native qualification ran on Linux/aarch64 with Xvfb in two configurations:
Xft plus SVG, and core fonts without SVG. Xm, Mrm, UIL, the gallery and Periodic
were rebuilt together. The final incremental builds took 14.67 and 12.10 seconds;
logs identify the rebuilt inputs and relinked targets. Content fingerprints,
build start/end times and test results are retained in the
[validation record](modern-integration-validation.json).

- **80 native test groups passed**, 40 in each configuration. These cover
  control geometry, live appearance, input and destructive callbacks; ColorPicker
  modes; symmetric rounding; arrow bounds; tree connections; Notebook flaps;
  Sidebar shortcuts; Column teardown; text replacement; bitmap pixel scaling; and tab scrolling/repaint.
- Notebook coverage includes **768 configurations per font build**. Periodic
  covers eight appearance/startup variants plus invalid options and missing UID.
- ABI checks passed for native compilation, header tracking, ELF linkage and
  runtime loading of ABI 9 libXm in both configurations. Periodic additionally
  exercises the rebuilt Mrm library and UIL assets.
- **3,890 of 3,910 source-contract checks passed.** The remaining 20 failures
  were also reproduced on the pre-integration maintenance branch. Their names
  are retained in the validation record; they are not counted as passing.

- Full-gallery smoke and live appearance runs passed in both builds: **94
  available component demos and 376 live transactions per configuration**.
  The runner supports an explicit bounded watchdog. The final live runs used
  600 seconds after a 240-second attempt timed out during concurrent host
  compilation; all interaction and geometry assertions remained enabled.
- Actual verified application processes produced 12 gallery captures across
  light/dark and scaled variants, plus light/dark Periodic captures. Executable
  paths, loaded library hashes and window ownership were checked before capture.
  Visual inspection confirmed the rounded focus band and full Periodic viewport.

All 105 component documentation screenshots were refreshed from the verified
final gallery build. ColorPicker and FileSelectionBox use taller capture windows
to show their complete examples. The screenshot manifest records every image hash
and its dimensions.

## Scope

This is acceptance of the implemented cohort. The broader visual-overhaul
roadmap, cross-architecture coverage, external input-method services and full
accessibility-service matrix remain separate work. Optional C++ prototype
packaging was checked statically; the prototype was not built. Historical development
checkpoints are retained in the [overhaul plan](modern-visual-overhaul.md).
