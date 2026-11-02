---
nav_exclude: true
---
# Modern appearance audit on macOS

The gallery's `--audit-modern DIRECTORY` mode visits every manifest entry in
modern light and modern dark, inventories render-table font types and visible
label extents, and records normal, focused, disabled and supported popup states.
The directory must already exist. Captures come from the production gallery's
actual X windows; each standalone popup has its own capture. Service classes and
optional components remain explicitly identified in the manifest.

Use an installed native build with Xft, Unicode text and SVG support, including
`--enable-demos`. Keep a single gallery instance on the user desktop. Automated
interaction fixtures run on a separate Xvfb display and do not send desktop input.
A build receipt must identify source/assets, executable and loaded library hashes;
an isolated fixture does not replace inspection of the running XQuartz gallery.

## Control behavior

- DataField accepts Xft renditions in UTF-8 locales instead of rejecting them for
  lacking an XFontSet. Editing, selection and character conversion continue to
  use the existing Unicode path.
- Compound-string drawing honors disjoint clip rectangles as well as partially
  intersecting ones, so separate contrast passes cannot repaint each other. See
  [Compound-string clip rectangles](xmstring-clip-regions.md).
- ProgressBar exposes `XmNrenderTable`, copies its supplied table, measures its
  preferred size and percentage text, and uses a rounded track in modern profiles.
  Text over the filled region chooses black or white from the actual fill color's
  luminance; the remaining text uses the ordinary foreground. Existing widget
  record layouts are unchanged.
- DatePicker and TimePicker use measured preferred sizes, flat field frames and
  visible disclosure/spinner controls. TimePicker's spinner changes its selected
  hours, minutes or seconds field; keyboard editing remains available. The focused
  field is underlined. DatePicker opens a full-sized calendar aligned below the
  owner, flipping above it when space requires, and clamps to the screen.
- Calendar sizes heading, weekdays and day cells from font metrics and scale.
  Modern selected dates use an accent surface with readable text. Header chevrons
  navigate months using the same range checks as keyboard navigation.
- ProgressRing and Spinner use the profile accent for theme-owned normal roles.
  Explicit foreground colors and semantic appearance roles retain precedence.
- Links use the profile's link color. InfoBar supplies severity-specific surfaces
  and text for normal-role defaults; explicit application colors still win. Toast
  has a flat frame. IconGadget borders use the modern flat renderer.
- Tab labels restore their own drawing color after disabled painting. No static
  saved color is shared between tab widgets or displays.
- Empty FormSection footers stay hidden and reserve no footer space.
- Automatic scrollbar drag scrolling registers zero data targets. It retains
  enter/leave timer behavior without treating a native pointer as an X Atom.
  Navigator notifications use a watched snapshot, so a callback can remove a
  navigator or destroy its frame without visiting freed list entries. ScrollBar
  dispatches a public callback snapshot and stops after widget destruction,
  including immediate destruction from timers; see
  [callback ownership](scrollbar-callback-snapshot.md).
- Gallery feedback starts with measured content and spans its available row;
  switching away from passive examples cannot leave a one-pixel label.
- ColorSelector accepts the valid Xt constructor form with no argument list;
  a null list is rejected only when its count is nonzero.

Classic geometry and painting remain available through the classic profile.
Application-specified dimensions remain authoritative; use preferred geometry
rather than a fixed legacy height for controls with modern font metrics.

The [macOS validation record](validation/macos-modern-ui-audit-20261102.json)
records native test groups, source/output fingerprints and actual-window findings.

## Acceptance

`check-modern-audit-controls.c` covers light/dark at 1× and 2×, UTF-8 Xft
DataField text, preferred field/calendar sizing, month navigation, time spinner
interaction and date popup open/close. `check-tab-color-state.c` reproduces the
previous disabled-to-normal tab contrast failure. Existing button, field, menu,
container, avatar, font-selector and profile regressions supplement these checks.

Font inventory and clipping checks are necessary but cannot judge layout or
appearance alone. Review the normal/disabled/focus captures and popup captures,
then inspect the exact verified process on XQuartz. Native clipboard, input-method
and accessibility qualification are separate from this visual audit.

Run the native suite with an installed prefix and its matching build tree:

```sh
python3 tests/automated/run-modern-macos-audit.py \
  --prefix /Volumes/S8T/tmp/your-build/install \
  --build /Volumes/S8T/tmp/your-build/build \
  --out /Volumes/S8T/tmp/your-build/native-audit
```

The output directory must be fresh. The runner includes the existing autoscroll
lifetime cases, color-override tests and light/dark/high-contrast UIL role tests.
