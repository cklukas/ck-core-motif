# Gallery layout and dialog artwork validation

The gallery refresh preserves native Motif controls and beveled surfaces while
using readable render tables, consistent page spacing, a searchable navigation
tree, populated examples and explicitly sized preview viewports.

The new error, information, question, warning and working symbols are original
40-pixel SVG assets embedded in libXm. SVG builds select them automatically;
non-SVG builds retain bitmap symbols and explicit application pixmaps retain
precedence. See [SVG images](svg-images.md) for the selection contract.

## Acceptance

- Display-backed gallery integration visits all 94 available live classes and
  verifies navigation, generated source, callbacks, keyboard traversal, focus,
  disabled-button visibility, preferred-size preview allocation and canvas
  containment after native geometry events. One optional
  class is unavailable; all 105 manifest records are checked.
- Gallery `make check` validates generated source provenance and catalogue links.
- Native dialog tests pass with SVG enabled and disabled. They check all five
  default symbols and custom pixmap overrides; SVG tests also check cache reuse,
  2x dimensions and background compositing. The SVG file-loader regression passes.
- Appearance profile and live appearance regression fixtures pass. The gallery's
  optional interactive Theme/DPI controls remain outside this qualification.
- Source checks cover manifest completeness, live examples, page spacing,
  startup profile selection, screenshot process readiness and tree disclosure state.

## Reproducible visual evidence

The publication manifest binds the screenshots to the component manifest,
actual gallery executable and actual loaded libXm SHA-256 fingerprints. Captures
use modern-light, scale 1.0, an X11 1440×1000×24 display and the final native
client window after its ready signal. Each scenario runs separately; its process
is stopped before the next one. These are native application captures, not
reconstructed mockups. The capture receipt and build timing records are retained
with the current verified build on the qualification host.

The clean SVG build uses Xft and librsvg, with `--enable-xft --enable-svg
--disable-static` and debug compiler flags. Incremental checks rebuild the
changed sources before capture. The gallery executable and dynamically loaded
libXm have independent build records, and both identities are verified.
