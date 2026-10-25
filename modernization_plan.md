# CK Core Motif modernization plan

## Purpose

Modernize CK Core Motif as a maintained native Xt toolkit without replacing its
widget model, resource database, callback contracts, UIL support, or existing
applications.  The target is a more capable and pleasant toolkit for new code,
including an additive set of missing general-purpose widgets, while retaining
the APIs and behavior that existing Motif clients deliberately use.  These are
ordinary Motif updates, not a separately branded or parallel widget library.

This is a plan, not a claim that every item is implemented or qualified.
Each accepted change must state its supported configuration and platform scope,
have an installed-library regression appropriate to the change, be recorded in
`CHANGES`, and update the applicable documentation.

## Non-goals and invariants

- Do not rewrite the toolkit around another UI framework or graphics backend.
- Do not remove, rename, or silently reinterpret existing public resources,
  callbacks, widget classes, UIL behavior, or geometry contracts.
- Do not claim that Xt widgets are generally thread-safe.  Widget creation,
  destruction, resource operations, callbacks, Xlib interaction and rendering
  continue to belong to the owning Xt UI thread.
- Do not turn an incomplete optional feature into a default-on claim.  Missing
  dependencies must either disable an optional facility clearly or make its
  requested configuration fail at configure time.
- Do not confuse a new extension's unit test or a rendered screenshot with
  qualification of an installed toolkit or real user-facing accessibility.
- Keep classic Motif appearance and resource configuration usable.  A modern
  profile is opt-in and does not replace application-provided resources.

The current fork already provides important modern foundations: optional Xft,
ICU and HarfBuzz text support; native UTF-8/grapheme/bidi facilities; optional
AT-SPI transport; XPM/PNG/JPEG image loading; and focused high-DPI fixes.  This
plan builds on those foundations rather than duplicating or bypassing them.

## Workstreams

### 1. Appearance, theme profiles and visual accessibility

The highest-value gap is a supported appearance system.  The historical
`XmThemes` implementation is not currently shipped, so a request for themes
currently fails explicitly rather than supplying a partial feature.

Implement an opt-in appearance-profile facility which resolves to ordinary
Motif resources.  A profile must be selectable at application/display setup
time and supply a coherent palette, typography defaults, spacing, border
metrics, button and field treatment, disabled state, selection colors and
keyboard-focus treatment.  The initial profiles should be:

- `classic`, preserving normal Motif defaults;
- `modern-light`, with restrained/flat surfaces and high-contrast focus;
- `modern-dark`, only after its contrast and image behavior are verified; and
- `high-contrast`, with contrast and focus visibility treated as functional
  requirements rather than decoration.

Profiles must use the existing resource database and normal application
overrides.  They must not add a global process-wide mutable style object that
can change a realized widget behind an application's back.  New resources, if
needed, are additive, documented, and have defined precedence relative to X
resources and explicit widget values.

Deliver a native visual/interaction fixture for representative Label,
PushButton, ToggleButton, TextField, List, ScrollBar, Menu, Dialog and
Disclosure states.  It must check focus traversal and contrast-relevant colors
in addition to image output.  Test normal, explicitly scaled and high-contrast
profiles on an isolated X server, then inspect a real window before making a
user-facing quality claim.

### 2. Scalable assets and display scaling

Keep the existing XPM/PNG/JPEG loading and pixmap cache contracts.  Add
optional SVG rasterization through the same image-to-pixmap path, subject to a
small, maintained dependency choice and strict image-size, decode-time and
allocation limits.  A caller using an existing pixmap resource should not need
to adopt a separate widget or rendering API just to load an SVG.

Cache SVG results by display, visual/depth, requested physical size, effective
scale and relevant symbolic-color inputs.  Release cache entries using the
same ownership rules as existing image entries.  SVG parsing must be treated as
untrusted input: reject unsupported/unbounded dimensions, recursive external
loads and arithmetic overflow; test malformed input and cache retirement.

Audit default interactive metrics beyond the existing indicator and arrow work:
focus rings, scrollbars, menu indicators, text cursor/selection geometry,
notebook tabs and standard dialog controls.  Preserve explicit application
dimensions.

XLibre 25.1 supplies a useful optional contract: its RandR outputs expose a
writable 32-bit integer property named `DPI`, where 96 is 1x, 144 is 1.5x and
192 is 2x.  Output-property events report changes.  That property is a scale
hint; it does not itself change the server's coordinate system, resize windows
or make other toolkits scale.  CK Core Motif can therefore support it as a
versioned XLibre integration, with an explicit per-shell output-selection and
change-notification policy.  It must retain the existing global Xft.dpi/default
behavior on X servers without that contract.

### 3. Modern language ergonomics without changing libXm

The C/Xt resource and callback model remains the primary ABI.  Improve new C++
consumer ergonomics in a separately versioned, optional `ckmotif++` support
library rather than by changing `libXm`'s object model.

The library should provide narrow RAII wrappers for owned Motif values such as
`XmString`, callback registrations and temporary argument lists; move-only
ownership where appropriate; typed resource construction helpers; and
non-owning `Widget` views.  It must support widgets made by ordinary C/Xt code,
must not assume it owns every widget, and must ensure that a C++ exception never
crosses an Xt callback boundary.

Start with a small C++17 baseline and normal C headers under `extern "C"`.
Avoid a second declarative widget hierarchy, implicit event loop, global
lifetime registry or a wrapper that makes raw `Widget` escape impossible.  Add
C++ SDK compile/link tests, runtime callback/destruction tests and examples
that build against an installed prefix.  A small, typed C argument-builder API
may be considered separately, but only if it demonstrably reduces unsafe
`ArgVal`/varargs use without obscuring normal Xt resource semantics.

### 4. Unicode, input methods and accessibility completion

Continue the native ICU/HarfBuzz/Xft implementation rather than introducing a
parallel text engine.  Existing immutable Unicode maps, shaping captures and
native composition state form the extension point.

Priority qualification work is:

- external input-method service composition, cancellation, restart and failure
  recovery across supported desktop environments;
- a maintained font/fallback corpus covering combining scripts, emoji,
  right-to-left text, CJK and constrained wrapping;
- clipboard/INCR lifetime and Unicode interoperability under the selected Xt
  dependency cohort;
- x86_64 builds and installed-library runtime validation in addition to native
  ARM coverage; and
- strict sanitizer work that distinguishes fixes in this toolkit from defects
  or accepted limits in system dependencies.

Expand the AT-SPI bridge by completing native Menu, List and editing semantics,
then test actual screen-reader journeys in addition to D-Bus protocol fixtures.
The semantic-provider API remains the correct path for custom canvases and
application-owned virtual content.  Do not manufacture hidden widgets merely
to satisfy an accessibility tree.

### 5. Layout correctness and hierarchy performance

Do not replace `XmForm` or alter its attachment semantics.  First establish a
reproducible benchmark and diagnostic corpus for deep/nested Forms, resize
storms, map/unmap, child insertion/removal, scroll containers and callback
reentrancy.  Record geometry-request counts, relayout passes, damage/redraw
work and elapsed time; use those measurements to identify the actual source of
cost before caching or batching.

Potential compatible corrections include coalescing redundant geometry passes,
avoiding repeated traversal of unchanged constraints, and producing optional
diagnostics for conflicting or cyclic attachment requests.  Each must preserve
Xt geometry negotiation and visible child bounds.

If an installed-client requirements corpus shows that common application
layouts remain unnecessarily difficult after that work, add a small opt-in
family of layout-manager widgets rather than changing `XmForm` behavior.  The
initial candidates are Stack (vertical/horizontal), Flex/Flow and Grid;
Overlay and an improved split manager are optional follow-ons.  Every manager
must have a narrow, documented geometry contract and support keyboard focus
order, resizing, internationalized direction, accessibility, geometry
negotiation and destruction/reentrancy tests from its first release.

### 6. Worker integration, not thread-safe widgets

Most applications need to perform background work, but making legacy Xt widget
records concurrently writable would not be safe or compatible.  Instead,
consider an additive application-context completion API:

- a worker creates a result owned independently of widgets;
- a thread-safe post operation wakes the owning Xt application context;
- the completion callback runs only on that context's UI thread;
- cancellation and application-context shutdown prevent delivery to retired
  widgets; and
- callers retain explicit ownership of result payloads until the documented
  handoff point.

The implementation must not call Xlib/Xt widget APIs from the worker and must
be stress-tested for repeated post/cancel/shutdown races.  Existing applications
can continue using their own pipes, work procedures or event-loop integration.

### 7. Additive standard widgets

Extend the toolkit with carefully scoped, resource-driven Xt widgets where a
widely useful control is absent from classic Motif.  Each new widget is an
additive public class with its own documented header, resources, callbacks,
UIL/WML support where applicable, installed-library tests and accessibility
semantics.  It must integrate with normal Xt geometry negotiation, keyboard
traversal, resource precedence, destruction/reentrancy and the owning UI
thread.  Existing widget classes, resources, callbacks, renderers and behavior
remain unchanged.

The initial component set is ToggleSwitch (including its label treatment),
ProgressBar, Spinner and, if justified by the same contract, ProgressRing,
NumberBox/Stepper, segmented control, SearchBox, ColorPicker, RatingControl,
InfoBar, Chip/Tag, Badge, AutoSuggestBox, BreadcrumbBar, lightweight flyouts
and improved tooltips, an improved TreeView, Calendar/DatePicker/TimePicker
and, after a separate design gate, a DataGrid/TableView.  The next additive
tier covers icon-and-text Labels, Avatar, text Divider, prompt TextField,
Link, labelled Scale, DisclosureGroup, FormSection, empty state, command
palette, toast, skeleton loading, richer context-menu/row-action and drag
feedback, plus bounded application-chrome composites.  The work is deliberately
sequenced from small controls to composite controls and then to the two
substantial view widgets.  New controls are part of CK Core Motif's normal
additive evolution; no alternate product name, profile or application model is
introduced.  Existing SecureField, Notebook/TabStack, menus, dialogs, basic
buttons and other established controls are retained rather than reimplemented.

This workstream excludes media playback, ink input, browser embedding, charts,
gauges, barcodes, a GPU scene graph, a general virtualization engine and
adaptive application-navigation chrome.  Those require materially different
ownership, rendering or application architecture and must be evaluated as
separate libraries or applications rather than smuggled into widget work.

## Work packages

Each package is an independently reviewable and finishable unit.  A later
package must not start until its stated dependencies have a recorded result.
An unsuccessful investigation is still complete when it leaves its benchmark,
decision and limits in the repository; it must not be silently converted into
an implementation promise.

### WP-00: Reproducible modernization baselines

**Depends on:** none.

**Objective:** establish a small, installed-prefix baseline for visual state,
keyboard navigation, high-DPI metrics and deep-hierarchy layout before changing
appearance or geometry code.

**Steps:**

1. Select one native X server, window manager, font set and explicit Xft.dpi
   values for the baseline; record their versions and paths.
2. Add fixtures that construct representative controls and a nested Form/scroll
   hierarchy, then record final geometry, focus traversal, X-visible
   configure/expose/focus activity and the selected display resource values.
   Internal geometry-request, relayout and redraw counters are deliberately
   deferred until the toolkit has a documented instrumentation contract.
3. Capture the actual verified process window for normal, focused, disabled,
   selected and scaled states.  Do not substitute a library-only render.
4. Run the fixtures from a fresh installed toolkit prefix and prove the loaded
   headers/libraries are the prefix under test.
5. Record source/asset hashes, configuration, output path and the results in
   the relevant validation documentation.

**Complete when:** the fixtures are deterministic on the selected environment,
their measurements are retained as a baseline rather than an invented
performance target, and they fail when deliberately pointed at the wrong
library prefix.

**Status (2026-10-25):** implemented and qualified on the AArch64 `uconsole`
Linux host with Xvfb.  Fresh installed-prefix runs passed at Xft.dpi 96 and
144; each receipt records the fixture/library/result/image hashes.  A fresh
full source-tree build and install now pass after correcting the bounded
text-selection compile error, the WML build graph, and UIL literal-only module
output; the focused WML and zero-topmost regressions remain recorded in the
repository. A fresh x86_64 out-of-tree source-tree build also passes after
removing the Exm demonstration library's case-insensitive `String.h` collision
with the C `<string.h>` header; legacy demo warnings remain explicit.

**Does not include:** appearance changes, performance optimizations, internal
instrumentation without a defined contract, or a claim that the baseline
represents all X servers/desktops.

### WP-01: Appearance-profile contract and precedence

**Depends on:** WP-00.

**Objective:** specify a compatible, resource-backed appearance-profile
mechanism before implementing a new visual style.

**Steps:**

1. Inventory existing class/default resources for the representative widgets in
   WP-00 and identify which values a profile may provide.
2. Define profile selection at application/display setup, the available profile
   names, the lifetime of a selected profile and failure behavior for an unknown
   profile.
3. Define and test precedence: explicit widget arguments override ordinary X
   resources, which override profile defaults, which override inherited Motif
   defaults (or another documented ordering if source evidence requires it).
4. Specify how profiles affect colors, fonts, spacing, borders, focus and
   disabled/selected states without introducing a mutable global style object.
5. Publish a short API/resource reference and migration example.

**Complete when:** a minimal `classic` profile is selectable, produces the
existing default appearance, precedence is covered by installed-library tests,
and the contract has no hidden dependence on widget-private records.

**Status (2026-10-24):** implemented and qualified on the AArch64 `uconsole`
Linux host with a fresh installed prefix under Xvfb.  The additive per-display
API exposes only the `classic` profile in this work package, stores selection in
the existing X resource database, rejects unknown values, and leaves realized
widgets unchanged.  The installed consumer test passed resource-versus-explicit
width precedence and the default/selected/rejected profile cases.

**Does not include:** a new visual profile, runtime style switching of realized
widgets or a replacement for application resource files.

### WP-02: Modern-light and keyboard-focus profile

**Depends on:** WP-01.

**Objective:** deliver the first visibly modern opt-in profile while retaining
classic Motif as the default.

**Steps:**

1. Choose documented palette, typography, spacing, border and focus metrics
   with measurable contrast targets.
2. Apply them through WP-01's resource path to Label, button/toggle, TextField,
   List, ScrollBar, Menu, Dialog and Disclosure; identify gaps that cannot be
   expressed through resources.
3. Implement only narrowly scoped additive renderer/resource corrections needed
   for those gaps, preserving explicit application values.
4. Exercise mouse and keyboard focus traversal, selection, disabled states and
   default/cancel dialog controls under the profile.
5. Compare geometry and focus behavior against WP-00; investigate any change
   outside documented metric differences.

**Complete when:** `modern-light` passes visual/interaction fixtures, has
visible keyboard focus, documented contrast decisions and no effect unless
selected.  `classic` continues to match the WP-00 resource baseline.

**Status (2026-10-24):** implemented and qualified on the AArch64 `uconsole`
Linux host with a fresh Xft-enabled installed prefix under Xvfb.  The opt-in
`modern-light` profile supplies documented palette, focus, padding and spacing
defaults through an Xrm database merged without overriding application
resources.  The installed fixture passed realized widget color checks,
explicit and ordinary resource precedence, control metrics and keyboard focus
traversal.  The WP-00 classic baseline was rerun and retained its prior result
and image hashes.

**Does not include:** dark mode, live style mutation, SVG loading or a promise
of platform-native desktop theming.

### WP-03: High-contrast and modern-dark profiles

**Depends on:** WP-02.

**Objective:** add contrast-sensitive profiles only after the normal profile
has proved that the profile mechanism works across core states.

**Steps:**

1. Define high-contrast foreground/background, focus, selection and disabled
   colors for every state exercised by WP-02.
2. Add automated contrast checks and actual-window tests for those states.
3. Add `high-contrast`; add `modern-dark` only if image/pixmap and disabled
   state handling pass the same tests.
4. Check that application-supplied colors and pixmaps keep documented
   precedence rather than being recolored unexpectedly.

**Complete when:** high-contrast is fully tested and documented.  Dark mode is
complete only if it is equally tested; otherwise record it as deferred without
advertising it as available.

**Status (2026-10-24):** implemented and qualified on the AArch64 `uconsole`
Linux host with a fresh Xft-enabled installed prefix under Xvfb.  The opt-in
`high-contrast` profile defines black/white text surfaces, yellow focus,
blue selection/arm colors, disabled-state foreground and three-pixel focus
metrics through the same Xrm fallback path.  The installed fixture passed
calculated contrast targets, realized colors, application precedence and
keyboard traversal.  The opt-in `modern-dark` profile is now equally
qualified: its fixture passed dark/disabled colors, explicit white precedence,
keyboard traversal, realized state and application pixmap identity.  See
`docs/modern-dark-profile.md`.

### WP-04: Scaled control-metric audit

**Depends on:** WP-00; WP-02 is recommended but not required.

**Objective:** extend high-DPI improvements based on measured default-widget
behavior while preserving explicit pixel dimensions.

**Steps:**

1. Test default and explicit values at the chosen 1x, 2x and 4x Xft.dpi inputs
   for focus rings, scrollbars, menu indicators, text selection/cursor geometry,
   notebook tabs and standard dialog controls.
2. Classify each finding as a rendering defect, an inappropriate unscaled
   default or intentionally application-owned geometry.
3. Correct only unscaled defaults; retain every explicit resource value exactly.
4. Add boundary tests for invalid, fractional, extreme and changing DPI inputs.
5. Document the per-display/screen contract and limitations.

**Complete when:** every corrected control has an installed fixture proving
default scaling and explicit-size preservation.  Per-output behavior remains a
separate optional package below.

**Status (2026-10-24):** the global-DPI subset is implemented and qualified on
the AArch64 `uconsole` Linux host through the installed widget regression.  The
fixture covers 1x, 2x and 4x `Xft.dpi` defaults for unlabelled ToggleButton /
ToggleButtonGadget indicators and SpinBox arrows, malformed/non-finite input,
saturation and explicit-size preservation.  The contract is finite,
positive, fully parsed `Xft.dpi` with a 75-DPI baseline; see
`docs/dpi-defaults.md`.  Focus rings, menu indicators, notebook/dialog
metrics, text cursor geometry and live reflow remain audit items rather than
unverified claims.  Per-output behavior remains WP-04A.

**Status (2026-10-24):** the installed control-metric audit is now implemented
and qualified on AArch64 `uconsole` under Xvfb.  It records PushButton and
TextField focus/shadow/margin values, ScrollBar dimensions, Notebook frame and
back-page metrics, MessageDialog margins, and a real TextField selection at
75/150/300 DPI.  Explicit PushButton highlight and ScrollBar width values are
preserved.  The observed constant-versus-scaled values are documented as an
audit classification; no additional source correction is claimed.  See
`docs/dpi-control-metrics.md`.

The additive live display-scale transaction now also rejects nested scale
changes from a callback, preserving the outer transaction's override and
retiring its per-display guard with the Motif display. The installed
`run-display-scale-reentrant.py` fixture is the focused regression. The
manifest-driven gallery then qualifies the realized historical-widget cohort
through the public transaction at 1.0, 1.5, 2.0 and 3.0 scales: all 94
manifest scenarios pass in the 16-cell matrix (1,504 captures, with hashes
and receipts retained). This closes the simulated historical-widget matrix;
real XLibre output changes and the separately listed metric-audit limits
remain open under WP-04A.

### WP-04A: XLibre per-output scaling integration

**Depends on:** WP-04.

**Objective:** support XLibre 25.1's per-output RandR `DPI` property without
making an unportable assumption about ordinary X11/Xorg desktops.

**Steps:**

1. Define the supported server predicate and verify the property using RandR:
   exactly one `DPI` value, type `INTEGER`, format 32, finite supported range,
   and an XLibre version/configuration that documents the property.  Treat a
   missing, malformed or unknown property as unsupported, not as a zero scale.
2. Define output selection for each top-level shell.  Start with the output
   containing the shell client area's centre; define deterministic tie-breaking
   and behavior when no output contains it.  All descendants use their shell's
   selected scale, so a window spanning outputs does not have contradictory
   per-widget coordinates.
3. Subscribe to RandR screen, output and output-property changes.  Re-resolve
   the shell's output and scale only on the owning Xt UI thread; coalesce a
   burst of configuration events into one reflow.
4. Define a scale-change transaction: preserve explicitly supplied pixel
   resources, recompute only documented default/derived metrics, refresh Xft
   font/render contexts and scale-keyed pixmap entries, relayout once, then
   issue one documented application notification after stable geometry exists.
5. Define lifecycle behavior for unrealized shells, output removal, temporary
   invalid RandR data and a shell moved across the output boundary.  The safe
   fallback is the existing global scale, never a guessed physical DPI.
6. Test 96/144/192 values, live property modification, output removal/addition,
   a spanning window, explicit metric preservation, font/pixmap replacement,
   keyboard focus, selections and destruction during a queued update on a
   real multi-output XLibre session.
7. Document this as an XLibre-specific optional feature, including its server
   version/probe, chosen shell-boundary policy and fallback on non-XLibre
   servers.

**Complete when:** the installed library passes the live multi-output fixture,
window geometry and resource precedence remain coherent through each scale
change, and ordinary Xorg/X11 runs retain the previous global-DPI behavior.

**Does not include:** claiming that generic RandR, Xft.dpi, output transforms
or physical monitor dimensions constitute a portable per-output scale contract.

**Status (2026-10-24):** phase 1 is implemented and qualified.  The optional
`--enable-xlibre-dpi` build detects Xrandr development dependencies at
configure time, validates XLibre's `DPI` output property (INTEGER/32-bit,
single finite value, bounded range) and safely rejects ordinary X servers.
The installed fixture passes valid/invalid parser cases and the non-XLibre
Xvfb fallback on `uconsole`.  It also supports running against a real XLibre
display, where every reported output must provide a valid property.  Shell
output selection, RandR event subscription, live reflow and explicit-resource
transactions remain phase 2 and are not advertised by this increment.

**Phase-2 progress (2026-10-25):** the internal shell-boundary selector and
the opt-in live observer are now implemented and installed-fixture qualified.
The observer subscribes to RandR screen/output/property notifications, drains
them on the owning Xt application context, coalesces a burst into one work
procedure, re-resolves the shell output and invokes one normal Xt resize before
the change hook. It chooses the output containing the shell client-area center,
falls back to the nearest output with a lowest-RandR-ID tie-break (using the
sum of horizontal and vertical distances), and retains global-DPI behavior for
invalid or unavailable properties. Explicit-resource preservation and the
derived-metric callback boundary are documented. A real multi-output XLibre
session is still required to qualify live property modification, output
removal/addition, spanning windows and destruction during a queued update.
The installed contract runner now uses a supplied real display for the shell
observer and checks two-shell registration, positive descendant scale lookup,
idempotent re-registration and retirement of one shell without invalidating
the other; its no-display Xvfb run remains an ordinary-server fallback.

The observer teardown contract now also removes its destroy callback before
freeing state on explicit unregistration. The installed observer regression
exercises unregistration followed by shell destruction, preventing a stale
callback closure from becoming a use-after-free path.

The queued work procedure now retains its observer state across the Xt resize
transaction. If reentrant application code destroys the shell, retirement
unlinks and cancels the observer while reclamation waits for the active work
reference; the optional change hook is not called after retirement. The
source-contract regression covers the retain/release ordering. A real
multi-output XLibre run is still required for live property changes and
destruction during queued reflow.

The RandR property boundary is now hardened as well: the decoder validates
type, format, item count and trailing data before reading Xlib's format-32
buffer, avoiding an out-of-bounds read for malformed format-8/16 replies. The
installed contract fixture covers the malformed-metadata ordering under the
sanitizer configuration. This closes a toolkit safety defect but does not
replace the required real multi-output qualification.
The AArch64 receipt and installed-library fingerprint are recorded in
`docs/xlibre-dpi-contract.md`.
The supported-server predicate also requires an exact `XLibre` vendor token;
the installed fixture covers versioned names and spoofed substrings.
Transient client-area, resource-query and allocation failures now return the
global-DPI fallback rather than preserving stale per-output state; the source
contract regression covers those early-return paths.
The installed runner now emits a `receipt.json` recording source/library
fingerprints, exact commands, display mode and elapsed time, keeping ordinary
Xvfb fallback evidence distinct from future real-XLibre qualification.
The opt-in `run-xlibre-live.py` fixture now exercises writable output-property
mutation and cleanup when a real display is supplied, while its Xvfb mode
returns an explicit skip rather than counting fallback as live evidence.
The companion source-contract regression locks in property restoration before
observer teardown, one-callback verification and the status-77 ordinary-Xvfb
boundary. Its runner also records source/library fingerprints and exact
commands in a receipt with an explicit pass/skip/fail result; this does not
substitute for a real multi-output XLibre run.

### WP-05: Optional SVG image loading

**Depends on:** WP-04.

**Objective:** load SVG assets through the established image-to-pixmap and cache
path without requiring applications to use a new widget API.

**Steps:**

1. Select a maintained SVG rasterizer and define it as an optional configure
   dependency with clear disabled/configure-failure behavior.
2. Add a bounded decoder adapter to the existing image cache; key cache entries
   by display, visual/depth, dimensions, scale and color inputs.
3. Reject unsafe dimensions, integer/size overflow, unsupported external asset
   retrieval and malformed input before allocation or rendering.
4. Preserve cache lookup/release behavior for XPM/PNG/JPEG and add fixtures for
   successful SVG load, scale changes, cache reuse/retirement and invalid input.
5. Test resource conversion via an ordinary pixmap-accepting widget and inspect
   the loaded result in a real window.

**Complete when:** an installed optional-SVG build passes valid/invalid/cache
fixtures, an SVG is usable through existing pixmap resource paths, and a
non-SVG build remains valid and clearly reports that SVG is unavailable.

**Status (2026-10-24):** implemented and qualified on AArch64 `uconsole`.
The opt-in `--enable-svg` build requires librsvg 2.52 or newer, reads SVG
documents through a bounded in-memory adapter, rejects unsafe dimensions and
unsupported visuals, and preserves the existing pixmap-cache ownership and
scale keys. The installed fixture passed intrinsic-size loading, scale-change
cache separation, cache reuse/reference retirement, malformed input and an
ordinary Xvfb window. A separately configured build without SVG linked without
the adapter and explicitly reported SVG as unavailable. See
`docs/svg-images.md` for the dependency and security contract.

### WP-06: Native text and external-IME qualification

**Depends on:** none; it may run in parallel with WP-01 through WP-05.

**Objective:** turn the existing Unicode/Xft/ICU/HarfBuzz implementation's
highest-risk remaining boundaries into explicit tested contracts.

**Steps:**

1. Define an external IME service matrix covering composition, cancellation,
   commit, restart and provider failure for each supported desktop setup.
2. Build a maintained font/fallback corpus covering combining scripts, emoji,
   RTL, CJK, long unbroken content and constrained wrapping.
3. Add installed-prefix tests that distinguish committed text, preedit state,
   grapheme movement/deletion, geometry and clipboard content.
4. Run the matrix first on the existing qualified architecture, then on x86_64;
   record unavailable services as gaps rather than passing substitutes.
5. Run focused sanitizer profiles and trace any remaining reports to toolkit,
   Xt/system dependency or external font-config ownership.

**Complete when:** the supported matrix and architecture results are documented,
new native failures have regressions or explicit limits, and the status page
does not overstate external-IME or sanitizer acceptance.

**Status (2026-10-25):** phase 1 is implemented and documented for the
qualified AArch64 Linux cohort. The installed native fixtures cover scalar and
grapheme movement, the maintained mixed-script/fallback corpus, local Compose,
real IBus XIM commit/cancellation and focused provider restart/loss behavior.
The large UTF-8 clipboard and interrupted INCR-transfer fixtures also pass
when Motif and its consumer use the pinned corrected Xt dependency. Direct
Motif-to-Motif Text/TextField owner/import round trips now pass for mixed-script
and multi-megabyte payloads, with an independent Xlib re-export check. The
system-Xt failure and exact dependency receipt remain recorded. The
`docs/native-text-ime-matrix.md` table records this bounded result and keeps
x86_64, broader provider/engine interoperability, candidate panels, drag-and-
drop workflows and sanitizer exit ownership as explicit open gates. A fresh
x86_64 installed cohort now passes the baseline, WML token, non-text Unicode,
accessibility-menu, XLibre DPI-contract, independent-client DnD and C++
fixtures (including the C++ ASan/UBSan run); this is recorded admission
evidence, not complete x86_64 text/IME or accessibility acceptance. The full
optional `demos/lib/Exm` tree now also compiles in that cohort after removing
its case-insensitive `String.h`/`<string.h>` collision; its historical warnings
remain outside the toolkit warning gate. The matrix does not treat unavailable
services or interposed callbacks as provider passes.

The native feature runner now accepts repeated `--runtime-library-dir`
arguments and records them in the receipt. This keeps the Xt dependency
boundary explicit: the interrupted clipboard-transfer and real IBus/m17n
service fixtures pass against the pinned corrected Xt cohort on `uconsole`,
while the same cancellation fixture exposes the known system-Xt owner-exit
failure. The external failure is retained as a dependency limitation rather
than hidden or counted as a toolkit pass.

The same runtime-directory contract is propagated into the local Compose
observer. Its normal installed-prefix fixture passes with the pinned Xt loaded
from the recorded dependency directory; strict execution reaches the fixture
body but remains bounded by the external Fontconfig ownership report.

That strict local-Compose run also exposed and now covers a malformed-keycode
path in `XmTranslateKey`: the virtual-key lookup validates the cached server
keycode range before indexing Xt's keysym table. A fresh installed AArch64
prefix passes the invalid-boundary consumer and the strict local-Compose
fixture; this is a toolkit safety correction, distinct from the remaining
external Fontconfig ownership gate.

The layout and line-context consumers now also call `FcFini()` only after
closing their Xft fonts and displays. Their strict results therefore do not
attribute Fontconfig's process-global configuration to the fixture or hide it
with a sanitizer suppression.

The DataField destroy path now frees the private render-table copy made during
initialization regardless of whether the source `XmNfontList` was supplied by
the application or obtained from the default table. This removes the toolkit's
Xft-rendition leak from the strict Unicode fixture. The remaining strict
Unicode result is a single 104-byte allocation rooted in the system Xt
selection-owner implementation; it is recorded as an external dependency
boundary rather than suppressed or misreported as a toolkit pass.

A strict installed FontSelector consumer was also qualified during this phase.
All observed Motif label destroy paths release their normal and insensitive
GCs, but the process still reports one 160-byte shared Xt/Xlib GC-cache
allocation rooted at `XtAllocateGC`/`XCreateGC`. This remains an explicit
dependency-ownership failure rather than a toolkit pass; see
`docs/fontselector-sanitizer-boundary.md`. A matching instrumented Xt/Xlib
qualification or a source-backed `libXm` reproducer is required before the
matrix-wide sanitizer gate can close.

The installed native cohort also includes the bounded `nontext-unicode`
public-API fixture for combining, non-BMP and Han values through Label, List,
ComboBox, Menu and FileSelection resources. The fresh 25-check run now also
iterates and validates both payloads and the explicit separator in a
two-segment compound `XmString`, exercises List existence, position and match
search with the non-BMP item, and round-trips a non-ASCII `XmNmnemonic`
resource. Physical-keyboard and application-level Unicode coverage remain
separate gates.

A strict consumer ASan/UBSan run of the interrupted-clipboard fixture also
passed against that cohort. The updated pinned Xt patch now covers both the
interrupted INCR value lifetime and the destroyed-widget selection-context
lifetime; the official helper-built cohort passes ABI, incremental-layout and
clipboard-lifetime with leak detection enabled. This is bounded dependency
evidence, not closure of the matrix-wide Fontconfig, application-DnD or
cross-architecture sanitizer gates.

The installed `check-dropsite-lifecycle` consumer now also covers the public
drop-site registration contract: operation and import-target retrieval,
callback identity, update bracketing, explicit unregister/re-register, and
implicit teardown of a registered widget. Three repeated Xvfb runs pass all
seven checks on `uconsole`; this is a bounded lifecycle result and does not
close cancellation, owner-replacement or application-DnD gates. The new
installed `check-dnd-transfer` consumer adds a real
same-process Motif drag gesture and passes conversion, valid-drop admission,
transfer-entry client-data propagation and selection-value ownership checks in
three repeated Xvfb runs. The installed `check-dnd-cross-process` consumer
now also passes one independent-client copy transfer in three fresh Xvfb
runs, qualifying the basic X11 client boundary without claiming owner
replacement or transfer cancellation. Multiple targets and application-DnD
journeys remain separate gates. The companion installed `check-dnd-cancel`
probe also passes the initiator-side `XmDragCancel` and finish-notification
contract in three repeated runs. The receiver-side `check-dnd-transfer-cancel`
consumer also passes delayed-start, active-incremental and requestor-shell
destruction cancellation in three fresh runs each. The source-side
`check-dnd-source-cancel` consumer
also passes owner replacement during an active drag and post-drop incremental
transfer cancellation in three fresh runs each; application-DnD journeys
remain open. The installed `check-dnd-multiple-targets` consumer also
passes a same-process two-target transfer, preserving each target's client
data and payload in three fresh runs; the cross-process mode now covers the
same two-target contract and a 4 MiB incremental transfer, while full
application journeys remain open. See
`docs/dropsite-lifecycle.md` and `docs/dnd-transfer-lifecycle.md`.

The x86_64 cohort now also passes the bounded installed text subset
(`incremental-map`, `preedit`, `layout`, `line-context`, `unicode-widgets`,
`font-resize` and `large-text`) using an explicit, fingerprinted fontconfig
corpus. The runner records `--fontconfig-file` and its hash in the receipt.
The text-layout implementation now groups separate HarfBuzz clusters that
belong to one extended grapheme and batches Xft glyph requests so color-glyph
captures remain within the X11 Render request limit. The 1 MiB large-text
fixture passes with the color-emoji corpus; it records 1,073,152 bytes,
761,856 scalars and 696,320 graphemes per widget with no X error or paragraph
layout warning. This closes that bounded x86_64 text gate without claiming
physical-display, external-IME or complete architecture acceptance.
An isolated AArch64 `uconsole` rebuild of commit `1c2e4379` independently passes
the installed `abi`, `layout` and `large-text` fixtures; its host lacks the
Xrandr headers required for the optional XLibre-DPI adapter, so that focused
check intentionally excludes only that adapter.
The missing development dependency was subsequently installed on `uconsole`
(`libxrandr-dev`). A fresh adapter-enabled AArch64 build now passes the
installed XLibre parser, output-selection and ordinary-server observer
fixtures, plus the live-fixture fallback (which correctly returns `skip` on
Xvfb); the installed `libXm.so.8.0.0` SHA-256 is
`be55a4d7802d41dd75ee8de622f7cb315cd15d3d802aeb1359b11cab076444fd`.
This closes the missing-header build gate only; a real multi-output XLibre
session is still required for live property mutation, hotplug, spanning-window
reflow and queued-destruction qualification.

The x86_64 cohort also now passes the bounded large UTF-8 clipboard round trip,
the installed transfer-storage clipboard/merge/bounds/lifetime modes and the
cancelled-transfer observer when the pinned corrected libXt 1.3.1 prefix is
used. The unpinned system-Xt sanitizer allocations remain recorded as an
external dependency boundary; this does not close application clipboard/DnD,
other providers or matrix-wide sanitizer acceptance.

### WP-07: Screen-reader usability and semantic completion

**Depends on:** WP-06 for text-specific claims; basic provider work may start
independently.

**Objective:** finish the user-observable accessibility contracts that protocol
fixtures alone cannot establish.

**Steps:**

1. Inventory exposed roles, names, states, actions, selection and text events
   for Menu, List, Text, TextField, Dialog, Disclosure and ScrollBar.
2. Add native semantic fixtures for each missing state transition, including
   destruction/reentrancy and modal admission.
3. Run an actual AT-SPI client/screen-reader journey for keyboard navigation,
   menu operation, list selection and text editing; preserve concise diagnostic
   evidence.
4. Correct native semantics or transport lifetime only where the observed issue
   is a toolkit contract defect.
5. Document supported assistive-client journeys separately from untested
   desktop/screen-reader combinations.

**Complete when:** the selected journeys and native semantic tests pass from an
installed prefix and the documentation names their real scope and limitations.

**Status (2026-10-25):** the native AArch64 installed cohort now includes a
fresh `--enable-atspi --enable-xft --enable-unicode-text` prefix and a passing
bounded Orca/Speech Dispatcher/eSpeak journey.  The receipt verified the exact
`libXm.so.8.0.0` mapping, 23 AT-SPI cache objects, Unicode text-boundary and
selection calls, all required native-control utterances including bounded
list/menu announcements, synthesized PCM and clean exit of the owned
services.  The observer identity check was corrected
to the project's ABI 8 contract; it could no longer pass against an older
`libXm.so.6` installation.  `docs/accessibility-journey-matrix.md` records
the 37.02-second receipt with 113 utterances and the evidence boundary.
Complete x86_64 text/IME and accessibility journeys, all desktop
screen-readers, application journeys, complete List/Menu semantics and clean
sanitizer exit ownership remain explicit gates. The bounded x86_64
accessibility-menu fixture is recorded separately and does not imply a real
AT-SPI or screen-reader journey.

The focused installed semantic rerun on 2026-10-25 passed the strict
accessibility, text-event, List and Menu fixtures in 19.79 seconds using the
pinned Xt dependency cohort. It adds regression evidence without promoting
those bounded provider checks to complete desktop/application semantics.

### WP-08: Optional C++ support-library design

**Depends on:** none.

**Objective:** freeze the smallest useful C++ support-library contract without
creating a competing widget toolkit.

**Steps:**

1. Write examples of current C++ consumer pain points involving `XmString`,
   argument lists, callbacks and widget ownership.
2. Define C++17 types for owned Motif values, temporary argument construction,
   callback registrations and non-owning widget references.
3. Specify raw-widget interoperation, move/copy semantics, destruction order and
   the exception barrier for callbacks.
4. Prototype against ordinary C-created widgets without accessing private Xm
   records or introducing a global registry.
5. Compile the prototype as an SDK consumer of an installed prefix and run
   callback-destruction tests.

**Complete when:** the public header/API proposal, examples and lifecycle test
results are reviewed, and there is an explicit go/no-go decision for WP-09.

**Does not include:** integrating the prototype into `libXm` or claiming a new
OO object model.

**Status (2026-10-25):** the public design proposal is backed by the installed
`ckmotif++` header and C++17 lifecycle fixture. The fixture exercises
copied/moved `XmString` ownership, one-shot argument storage, non-owning
raw-widget interoperation, callback exception capture and
destruction-before-connection-release without private Xm records or a global
registry. Fresh GCC and Clang runs, including AddressSanitizer/UBSan with leak
detection, pass on the qualified AArch64 install; see
`docs/cpp-support-library-prototype.md` and `docs/cpp-support-library.md`.

### WP-09: Optional C++ support-library release

**Depends on:** WP-08 with a go decision.

**Objective:** ship the small C++ support library as a separately versioned,
optional SDK component.

**Steps:**

1. Implement only the types accepted in WP-08 and keep all public Xm calls in
   their normal C ABI.
2. Add build/install metadata that does not make C++ a libXm build requirement.
3. Add compile, link, callback exception-barrier, widget-destruction and mixed
   C/C++ ownership fixtures against an installed prefix.
4. Publish ownership and UI-thread rules with a short C++ example.

**Complete when:** a clean C build remains unaffected, the optional C++ SDK
passes installed-prefix tests, and the library never requires ownership of an
arbitrary raw `Widget` to be useful.

**Status (2026-10-25):** implemented and qualified on a fresh AArch64
installed prefix. The optional header-only `ckmotif++` package installs
`ckmotif++/motif_support.hpp` and `ckmotif++-8.pc` without adding a C++
requirement to `libXm`. GCC, Clang, and both sanitizer variants pass the
callback exception-barrier, destruction-order and mixed C/C++ ownership
fixture. The package remains subject to the separate toolkit-wide x86_64
qualification gate; see `docs/cpp-support-library.md` and
`docs/cpp-support-library-release-decision.md`.

### WP-10: Form and hierarchy diagnostic corpus

**Depends on:** WP-00.

**Objective:** identify actual layout costs and correctness failures before
altering Form's legacy negotiation behavior.

**Steps:**

1. Extend the WP-00 hierarchy corpus with resize storms, map/unmap, child
   insertion/removal, scroll containers and callback-triggered mutation.
2. Capture geometry requests, relayout traversals, damage/redraw work and
   final child bounds at each scenario.
3. Add optional diagnostics for conflicting/cyclic attachments only where they
   can be detected without changing layout decisions.
4. Establish reproducible negative controls for known redundant work or invalid
   constraint cases.
5. Publish the measured bottlenecks and a decision record naming the next
   focused correction, or conclude that no generic core correction is justified.

**Complete when:** the corpus can distinguish a deliberate regression from
normal system variation and identifies a concrete, source-backed target for
WP-11.

**Status (2026-10-24):** phase 1 is implemented and qualified on the AArch64
`uconsole` Linux host with a fresh installed prefix under Xvfb.  The installed
consumer now exercises resize storms, map/unmap, nested scroll/layout
containers, dynamic child insertion/removal and callback-triggered mutation.
It records public geometry-request/query observations, selected redraw calls,
Configure/Expose/Map/Unmap activity, final bounds and a receipt with source and
result hashes.  The interposition counters are explicitly comparative rather
than a complete accounting of private Form traversals.  The repeated
same-value margin update is a negative control and produced zero additional
geometry requests or Configure events on the qualification run.  No generic
Form correction is claimed yet; a measured hotspot and source-backed decision
are still required before WP-11 changes source behavior.

The callback-triggered scroll-container extension is now separately qualified:
auto-drag timers and payloads retire across leave, disable, scrollbar removal,
parent destruction and value-callback mutation, while navigator callback lists
remain safe when application code destroys the active frame.  This is a
lifecycle correction, not a change to Form attachment negotiation.  The
synthesized DropSMgr clipper records are also retired during owner teardown;
three repeated native runs and a strict consumer ASan/UBSan run pass all eight
scenarios and 81 checks.  The pinned Xt dependency patch now also retires the
selection context after widget-destroy/disown ordering; strict ABI,
incremental-layout and clipboard-lifetime consumers pass against the fresh
helper-built cohort.  Broader system-library ownership, application DnD and
other architectures remain explicit gates; see `docs/scroll-autodrag-lifetime.md`.

See `docs/form-layout-corpus.md` for the fixture schema, limits and current
qualification scope.

### WP-11: Focused Form/hierarchy correction

**Depends on:** WP-10.

**Objective:** fix one measured generic layout correctness or redundant-work
issue without changing attachment semantics.

**Steps:**

1. State the exact measured defect, its owning toolkit layer and the affected
   compatibility contract.
2. Implement the smallest coherent correction, such as coalescing redundant
   passes or avoiding traversal of unchanged constraints, only when the
   diagnostic corpus proves it is safe.
3. Test final geometry, Xt negotiation, focus order, resizing, child lifecycle
   and callback reentrancy against normal and negative-control cases.
4. Compare the same benchmark against WP-10 and investigate both unexpected
   speedups and slowdowns.
5. Document the behavior and retain the result as the baseline for a possible
   next iteration.

**Complete when:** the identified defect is fixed by installed-prefix tests and
all unmodified attachment behavior remains covered.  It is acceptable to close
the work package with no source change if the measurement disproves the premise.

**Status (2026-10-24):** completed as a no-source-change assessment on the
AArch64 `uconsole` host.  Two identical WP-10 installed-prefix runs produced
the same result hash and metrics; the no-op control produced zero additional
geometry requests or Configure events.  The corpus therefore identifies no
source-backed redundant pass or correctness defect to fix safely in `Form.c`.
Changing attachment evaluation speculatively would risk legacy Xt negotiation,
so this package is closed under its explicit no-change rule.  See
`docs/form-layout-decision.md` for the evidence and limits.

### WP-12: Decision and delivery of a new layout manager

**Depends on:** WP-10 and at least one completed WP-11 assessment.

**Objective:** add a new layout manager only if evidence shows that a recurring
layout need cannot be served safely by corrected existing managers.

**Steps:**

1. Write a decision record comparing existing Motif managers and the proposed
   grid/flex-like contract, including internationalized direction and geometry
   negotiation.
2. Define a new opt-in widget class, public resource/callback contract,
   keyboard-focus order and accessible representation; do not overload Form.
3. Implement independent multi-instance layout, resize, child mutation,
   destruction/reentrancy and accessibility fixtures.
4. Provide a minimal example and installed-prefix documentation.

**Complete when:** the new manager meets the component requirements, has no
behavioral effect on existing Form clients and passes its installed fixtures.

**Status (2026-10-24):** completed as a no-go decision.  WP-10/WP-11 provide
  deterministic nested-Form evidence but no reproducible consumer requirement
  that cannot be met by composing `XmForm`, `XmRowColumn` and existing
  containers.  Adding a second grid/flex geometry contract now would increase
  public API and accessibility/focus obligations without a source-backed need.
  See `docs/layout-manager-decision.md`; reopen this package only with a real
  installed-client corpus meeting the stated requirements.

### WP-13: UI-thread completion API investigation

**Depends on:** none.

**Objective:** decide whether existing pipes, work procedures and application
event-loop integration leave a generic background-work gap worth standardizing.

**Steps:**

1. Collect representative consumer requirements: producer threads, result
   ownership, cancellation, application-context teardown and latency.
2. Prototype a minimal result object plus thread-safe wakeup mechanism that
   delivers only on the owning Xt UI thread.
3. Stress repeated post/cancel/context-destroy races without performing Xlib or
   Xt widget calls from worker threads.
4. Compare the prototype with existing Xt mechanisms and record its cost,
   portability and lifecycle behavior.
5. Make a go/no-go decision for WP-14.

**Complete when:** the decision is backed by a runnable prototype and stress
evidence.  A no-go result is complete if existing mechanisms are adequate.

**Status (2026-10-24):** phase 1 prototype implemented and qualified on the
AArch64 `uconsole` host under Xvfb.  Four worker threads posted 320 owned
results through a POSIX pipe and `XtAppAddInput`; every result was validated
and freed on the UI thread.  A cancellation phase stopped a larger workload,
drained and freed queued results before input removal, and destroyed the
application context.  The fixture passed strict C11 compilation and reported
31 checks with no invalid results or posting failures.  Existing Xt input
integration is adequate for the demonstrated POSIX/X11 use case, but a
library-owned cross-platform completion contract remains uncommitted pending
real consumer requirements.  See `docs/ui-thread-completion-prototype.md`.

### WP-14: Optional UI-thread completion API

**Depends on:** WP-13 with a go decision.

**Objective:** provide one explicit, documented way to deliver independently
owned worker results to the owning Xt thread.

**Steps:**

1. Implement the accepted API with explicit result ownership, cancellation and
   application-context shutdown behavior.
2. Ensure no worker path touches widgets, displays or Xt callback dispatch.
3. Add stress fixtures for concurrent posts, cancellation, context retirement,
   widget retirement and callback reentrancy.
4. Build and run the API as an installed-prefix consumer under normal and
   sanitizer configurations where supported.
5. Document it as UI-thread marshalling, never as thread-safe widgets.

**Complete when:** the shutdown/cancellation race suite passes, the lifetime
contract is clear, and normal Motif clients have no new threading requirement.

**Status (2026-10-24):** no-go.  WP-13 demonstrates that existing
application-owned pipes plus `XtAppAddInput` provide a correct UI-thread
handoff for the qualified POSIX/X11 case.  No real consumer requirement in
this checkout justifies adding a process-wide queue, hidden worker, or new
cross-platform ownership contract to `libXm`.  See
`docs/ui-thread-completion-api-decision.md`; reopen only with the stated
consumer and lifecycle evidence.

### WP-15: Additive-widget contract and delivery harness

**Depends on:** WP-00.

**Objective:** establish one public, resource-driven contract for new widget
classes before individual controls add incompatible conventions.

**Steps:**

1. Inventory the applicable Xm public-class, resource, callback, child-
   ownership, traversal, geometry and UIL/WML conventions; publish the naming
   and versioning policy for additive classes without reusing an existing
   identifier or resource meaning.
2. Define common requirements for value changes, activation, cancellation,
   sensitivity, focus, rendering at explicit and scaled metrics, destruction
   and callback reentrancy.  Use typed public callback records where a generic
   Xt callback is insufficient; never change an existing callback record.
3. Add an installed-prefix component fixture harness that proves the loaded
   headers and libraries, resource precedence, keyboard-only operation,
   accessibility-provider semantics and teardown for a minimal new widget.
4. Define the documentation template: purpose, resource and callback tables,
   geometry/ownership rules, UI-thread rule, accessibility behavior, examples
   and limits.

**Complete when:** the policy and harness are reviewed against an installed
prefix, and the first component can use them without private Xm records or a
special application runtime.

**Does not include:** implementation of a user-facing control or a new
application-wide component framework.

**Status (2026-10-25):** the contract and installed-prefix harness are
implemented and qualified on the AArch64 `uconsole` host under Xvfb.  The
strict C11 fixture compiled and linked against the selected prefix, verified
explicit-over-fallback resource precedence, keyboard focus traversal, the
public accessibility-change callback, activation callback teardown and
application-context shutdown in 7 checks.  The existing PushButton and List
classes are contract probes; WP-16 must add its own class-specific semantics
and accessibility fixture.  See `docs/additive-widget-contract.md`.

### WP-16: ToggleSwitch

**Depends on:** WP-15.

**Objective:** add an opt-in binary switch control for settings-style choices
without changing `XmToggleButton` or its indicator semantics.

**Steps:**

1. Define the state/value, label treatment, sensitivity, keyboard and pointer
   contract, including its relationship to existing toggle-group/radio
   behavior.  A labelled switch is a standard presentation of this one class,
   not a second toggle API.
2. Implement a separate public widget class with resource-backed on/off
   rendering, explicit focus and disabled states, and callbacks that identify
   the new value without altering existing toggle callback structures.
3. Ensure a non-color indication of state and a native accessible role, name,
   checked state and activation action.
4. Test resource precedence, direct and keyboard activation, focus traversal,
   groups where supported, resize/scale behavior and destruction from a value
   callback against an installed prefix.

**Complete when:** the new control is usable as an independent additive class,
classic `XmToggleButton` fixtures retain their prior result, and the installed
fixture covers normal, disabled, focused and both value states.

**Status (2026-10-25):** implemented and qualified on uconsole.  The new
`XmToggleSwitch` class has dedicated public/private records and constructors,
typed state access and callback data, resource-backed label/state rendering,
keyboard/pointer activation, focus/disabled indication, and accessibility
check-box semantics with activate and change notifications.  It deliberately
does not participate in `XmToggleButton` radio/toggle groups; it is a
standalone binary control.  The installed-prefix fixture passed 17 checks,
including both states, disabled/focus behavior, accessibility query/action,
resize metrics, and destruction from a value callback.  See
`docs/toggle-switch.md` and `tests/automated/run-toggle-switch.py`.

### WP-17: ProgressBar, Spinner and ProgressRing

**Depends on:** WP-15.

**Objective:** add determinate progress feedback plus a simple indeterminate
Spinner and, only if it meets the same contract, an optional compact ring
presentation.

**Steps:**

1. Define a bounded determinate range/value model, textual progress exposure,
   indeterminate state and behavior for invalid ranges and updates.
2. Implement a new resource-driven bar widget and a distinct Spinner.  Give
   indeterminate motion a
   bounded Xt timer lifecycle that starts only while eligible for display and
   is cancelled before unmap/destruction; it must not create a worker thread or
   global animation loop.
3. Evaluate a ring as another presentation of that model; ship it only if
   sizing, focus policy, high-contrast output and accessible progress values
   remain equally testable.
4. Test minimum sizes, explicit dimensions, value transitions, timer teardown,
   high-DPI and high-contrast output, callback reentrancy and accessibility
   value changes from an installed prefix.

**Complete when:** the bar and Spinner have documented, accessible contracts.
A ring is included only with equivalent evidence; otherwise it is explicitly
deferred.

**Status (2026-10-25):** implemented and qualified on uconsole.  The additive
`XmProgressBar` exposes normalized bounded range/value state, determinate and
indeterminate rendering, typed value-change callbacks, explicit sizing and
progress accessibility semantics.  The distinct `XmSpinner` uses one bounded
Xt timer only while realized, managed, mapped, sensitive and active; it cancels
the timer on deactivation, unmanagement and destruction, and exposes busy
accessibility semantics without a numeric value.  The installed-prefix
fixtures passed 17 ProgressBar checks and 13 Spinner checks under Xvfb,
including repeated timer advancement/cancellation and callback re-entry.  The
additive `XmProgressRing` now supplies the equivalent compact presentation:
it shares the bounded range/value, label, callback and accessibility contract,
preserves explicit dimensions, uses DPI-derived defaults, renders determinate
and static indeterminate states, and has no animation timer.  Its installed
prefix fixture passed 15 checks under Xvfb, including range/value clamping,
callbacks, focus traversal, accessibility value/busy state and teardown.  See
`docs/progress-ring.md` and `tests/automated/run-progress-ring.py`.

### WP-18: NumberBox and Stepper

**Depends on:** WP-15.

**Objective:** add bounded numeric entry with both full spin controls and a
compact increment/decrement Stepper while preserving `XmTextField` and
`XmSpinBox` behavior for existing clients.

**Steps:**

1. Specify supported numeric domains, precision, step/page-step, minimum and
maximum, wrapping policy, editable text, locale/formatting boundary and the
distinction between tentative text and committed numeric value.
2. Build separate NumberBox and Stepper presentations over the same accepted
   value contract using documented public child ownership; do not depend on
   private `XmTextField` or `XmSpinBox` records.
3. Define validation, parse-error presentation, programmatic updates and value
   callbacks so that an invalid edit cannot silently become a different value.
4. Define an optional labelled-Scale presentation using existing `XmScale`
   behavior only through public APIs: visible minimum/maximum/value text is an
   additive companion, not a reinterpretation of `XmScale` resources.
5. Test keyboard and pointer stepping, direct entry, range boundaries, locale
   cases in the supported cohort, labelled-Scale synchronization, resource
   precedence, focus order, resize, callback/destruction reentrancy and
   accessible value semantics.

**Complete when:** an installed consumer can create, configure and validate a
NumberBox or compact Stepper without modifying an existing text or spin widget
contract.

**Status (2026-10-25):** implemented and qualified on uconsole.  The additive
`XmNumberBox` and `XmStepper` classes use a documented signed-integer contract
with inclusive bounds, step sizes, clamping/wrapping, explicit dimensions,
keyboard/pointer stepping and typed value callbacks.  NumberBox keeps tentative
ASCII decimal text separate from its committed value and reports parse/range
errors through its invalid callback; neither widget uses private
`XmTextField`/`XmSpinBox` records.  Installed-prefix Xvfb fixtures passed 15
NumberBox checks and 10 Stepper checks, including range precedence, boundary
behavior, focus/sizing, callback delivery and accessible values.  The optional
labelled-`XmScale` companion remains deferred until a public-child synchronization
contract and locale/formatting evidence are defined; the core NumberBox and
Stepper completion criteria are met without changing existing Motif widgets.

### WP-19: Lightweight flyouts, tooltips and teaching tips

**Depends on:** WP-15.

**Objective:** provide an anchored, non-modal popup primitive for contextual
content, including improved delayed tooltips and an optional teaching-tip
presentation, without changing dialog or menu-shell behavior.

**Steps:**

1. Define anchor placement, screen-edge adjustment, focus/admission policy,
   keyboard and pointer dismissal, modal exclusion, and owner/map/unmap/
   destruction lifetime.
2. Implement a separate popup/flyout contract using supported Xt shell and
   event mechanisms.  It must avoid grabs or event interception that change
   unrelated menu, dialog or shell behavior.
3. Add delayed/hover tooltips and an optional teaching-tip treatment only after
   delay/cancellation, arrow placement where applicable, readable text,
   accessible relation/announcement and dismissal behavior are defined.
4. Test nested shells, rapid show/hide, owner destruction, focus restoration,
   screen edges, keyboard-only use and callback reentrancy from an installed
   prefix.

**Complete when:** a flyout is safely composable by later controls and has no
observable effect on existing shell or menu fixtures.

**Status (2026-10-25):** the additive `XmFlyout` primitive is implemented and
qualified on uconsole.  It creates a transient, override-redirect popup below
its owner, clamps placement to the active screen, uses `XtGrabNone`, preserves
keyboard focus, and tears down on explicit hide, owner unmap/destruction or
flyout destruction.  Escape/Return and pointer activation dismiss it through a
typed callback without installing a global event loop or changing menu/dialog
admission.  The installed Xvfb fixture passed 5 checks across repeated runs,
including show/hide, dismissal delivery, edge-owner geometry and owner-unmap
cleanup.  `XmTeachingTip` now supplies the delayed, copied title/body layer
over this primitive with cancellation, edge clamping, non-modal dismissal and
panel accessibility semantics; its installed fixture passes 10 checks.
Existing Motif tooltip behavior remains unchanged.

### WP-20: ColorPicker standardization

**Depends on:** WP-15.

**Objective:** offer one documented ColorPicker without reinterpreting the
existing `XmColorSelector` extension or its clients.

**Steps:**

1. Audit the existing selector's public surface, limits and tests.  Retain it
   unchanged; record whether the new picker can adapt to it only through public
   APIs or must be a separate implementation.
2. Define the new picker's color representation, supported color spaces and
   alpha policy, committed-versus-preview callbacks, keyboard editing and
   palette/recent-color ownership.
3. Implement gamut/error presentation that never substitutes a color silently,
   plus non-color value readout and accessible names/values.
4. Test parsing/formatting, cancellation/commit, palette lifetime, explicit
   resources, high-contrast and scale output, destruction/reentrancy and
   installed-prefix accessibility semantics.

**Complete when:** the new control has a stable documented value contract and
the existing selector's API and behavior are unchanged.

**Status (2026-10-25):** the additive `XmColorPicker` is implemented and
qualified on uconsole.  It defines packed 8-bit RGBA values (`0xRRGGBBAA`),
separate preview and committed-value callbacks, explicit Return/Escape
commit/cancel behavior, a visible hex readout, colormap-backed swatch drawing
with a foreground fallback, and accessible numeric/value-text semantics.  The
installed Xvfb fixture passed 8 checks across repeated runs, including resource
precedence, preview-versus-commit behavior, explicit sizing and accessibility.
`XmColorSelector` remains unchanged; color-space conversion, gamut mapping and
palette/recent-color policy remain intentionally outside this compact contract.

### WP-21: RatingControl

**Depends on:** WP-15.

**Objective:** add a compact bounded ordinal-rating control without assuming a
particular glyph set or changing toggle-button groups.

**Steps:**

1. Define count, minimum/maximum, clear/readonly policy, integer and optional
   fractional selection policy, labels and accessible value text.
2. Implement a separate resource-driven control with keyboard navigation and
   activation, pointer selection, visible focus and a state representation that
   remains distinguishable without color or star artwork.
3. Test range boundaries, right-to-left presentation, scale/explicit metrics,
   disabled and readonly states, callbacks and destruction reentrancy from an
   installed prefix.

**Complete when:** applications can use a new accessible rating widget without
depending on undocumented collections of ToggleButtons.

**Status (2026-10-25):** the additive `XmRatingControl` is implemented and
qualified on uconsole.  It exposes a bounded integer value from zero through
`XmNratingCount`, explicit clear and readonly policy, keyboard/pointer
selection, visible focus, and outlined/filled segment rendering that does not
depend on star glyphs or color alone.  Its installed Xvfb fixture passed 8
checks across repeated runs, including explicit sizing, boundary clamping,
callbacks and accessible value semantics.  Fractional ratings and localized
value text remain outside this integer contract.

### WP-22: InfoBar

**Depends on:** WP-15.

**Objective:** add a dismissible inline information, warning and error banner
that does not alter dialog admission or application status handling.

**Steps:**

1. Define severity, icon/text/action ownership, closability, disclosure and
   dismissal callbacks, layout response to narrow widths and screen-reader
   announcement rules.
2. Implement a composite widget with public resource precedence and explicit
   focus policy: it may announce meaningful changes but must not unexpectedly
   steal keyboard focus.
3. Test all severities, optional action and close button, resizing, traversal,
   rapid updates, owner destruction and accessible live-message behavior.

**Complete when:** the new bar is an independently testable additive widget;
existing MessageBox, dialog and application-defined status areas are unchanged.

**Status (2026-10-25):** the additive `XmInfoBar` is implemented and qualified
on uconsole.  It exposes info/warning/error severity, message text, optional
action and close callbacks, Escape/pointer dismissal, explicit sizing and a
non-focus-stealing inline presentation.  Its accessibility provider exposes
the live message text as a panel.  The installed Xvfb fixture passed 7 checks
across repeated runs, including action/dismissal delivery, sizing and
accessibility; existing MessageBox and dialog behavior is unchanged.

### WP-23: AutoSuggestBox

**Depends on:** WP-18 and WP-19.

**Objective:** add text entry with an anchored suggestion list while preserving
ordinary `XmTextField`, `XmList` and ComboBox contracts.

**Steps:**

1. Define the suggestion provider/model ownership, stable item identity,
   filtering boundary, selection/commit/cancel callbacks, no-result state and
   explicit UI-thread rule for model updates.
2. Compose the control from public text/list and flyout facilities, with a
   documented popup focus and keyboard policy.  It must not make blocking I/O
   or invoke application providers from an Xt drawing path.
3. Define stale-result handling so that a delayed model update cannot replace a
   newer query or commit an unintended suggestion.
4. Test typing, composition/preedit boundaries where supported, pointer and
   keyboard selection, dismiss/reopen races, model/owner destruction, focus
   restoration, accessibility and resource precedence from an installed prefix.

**Complete when:** an application can supply and retire suggestions safely
without a global popup, private widget access or changed text/list semantics.

**Status (2026-10-25):** `XmAutoSuggestBox` is implemented as a per-instance
composition of the public `XmSearchBox` and `XmList` facilities.  It copies
suggestion IDs and labels, binds updates to a monotonically increasing query
generation, rejects stale/future results, provides keyboard/pointer selection,
explicit no-results and dismissal behavior, and restores focus to the search
field.  Its installed fixture passes 13 checks; providers and all model/I/O
work remain application-owned and UI-thread-bound.

### WP-24: BreadcrumbBar

**Depends on:** WP-15.

**Objective:** add path navigation as a resource-driven control rather than a
convention of ad-hoc buttons and labels.

**Steps:**

1. Define segment identity/label ownership, activation callbacks, separators,
   right-to-left order, current-location state, overflow/collapse behavior and
   accessible path semantics.
2. Implement a separate composite with public child lifecycle and geometry
   negotiation; it must not parse or impose an application's path format.
3. Test long and localized labels, narrow widths/overflow, keyboard traversal,
   segment mutation/removal, resize, callback reentrancy and destruction from
   an installed prefix.

**Complete when:** the bar provides deterministic navigation and overflow
behavior while existing button, label and menu widgets retain their contracts.

**Status (2026-10-25):** the additive `XmBreadcrumbBar` control is implemented
with copied labels and application-owned IDs, explicit max-visible ellipsis
collapse, logical RTL rendering, keyboard/pointer selection and activation,
and list/value semantics in the accessibility provider.  The installed-prefix
fixture covers segment replacement, current-index callbacks, bounded geometry
and accessibility; path parsing and navigation remain application
responsibilities.

### WP-25: Improved TreeView

**Depends on:** WP-15 and WP-19.

**Objective:** add a separate hierarchical view with modern selection and
expansion behavior without altering `XmTree`, `XmOutline` or `XmContainer`.

**Steps:**

1. Write a design record comparing the existing widgets and define the new
   view's model ownership, stable node IDs, expansion, selection, icons,
   keyboard traversal, optional incremental child retrieval and accessibility
   tree semantics.
2. Set a deliberately bounded viewport policy.  It may use lightweight
   realization/caching where measured, but it must not claim a general
   ItemsRepeater-style virtualization engine.
3. Implement only the accepted model/view contract and test large but bounded
   trees, lazy child availability, mutation while expanded, selection changes,
   resize, scale, right-to-left layout, destruction/reentrancy and installed
   accessibility behavior.
4. Benchmark against the WP-00 harness and record memory, geometry and redraw
   behavior before adding any caching or incremental realization.

**Complete when:** the new view passes its installed-prefix model, interaction
and accessibility suite, and existing hierarchy widgets are unchanged.

**Status (2026-10-25):** the additive `XmTreeView` is implemented with a
bounded flat node model (maximum 4096 nodes), copied labels, opaque stable IDs,
validated parent/child relationships, deterministic expansion and selection,
finite visible rows, wheel/top-row scrolling, RTL keyboard behavior and a
tree accessibility summary.  The installed-prefix fixture covers malformed
model rejection, mutation, expansion/selection callbacks, bounded scrolling
and accessibility; lazy retrieval and general virtualization remain deferred.

### WP-26: Calendar, DatePicker and TimePicker

**Depends on:** WP-19 and WP-15.

**Objective:** add explicit date/time entry controls with correct calendrical
boundaries rather than embedding date parsing in an application convention.

**Steps:**

1. Define calendar system, locale data source and fallback, week-start rules,
   date-only versus instant/time-zone semantics, minimum/maximum values,
   invalid/ambiguous local times and committed-versus-editing values.
2. Implement Calendar first, then an optional flyout DatePicker and TimePicker
   using the same value contract.  No component may silently apply the host
   time zone to a date-only value.
3. Test leap years, month boundaries, daylight-saving gaps/overlaps where
   times are supported, localized labels, keyboard navigation, range limits,
   flyout teardown, accessibility and resource precedence.

**Complete when:** the installed components have documented date/time
semantics, deterministic boundary behavior and no effect on existing text,
list, dialog or shell APIs.

**Status (2026-10-25):** the date-only `XmCalendar` phase is implemented and
qualified on the AArch64 `uconsole`.  It uses a proleptic Gregorian
1..9999 civil-date contract, validates leap years and inclusive ranges before
mutation, supports deterministic month/day keyboard navigation, week-start and
RTL presentation resources, date-change/activation callbacks, and a calendar
accessibility summary.  Locale providers and timezone interpretation remain
explicitly outside the date-only contract.

The additive `XmDatePicker` now reuses that contract through a bounded,
non-modal `XmCalendar` popup with validated ranges, screen-clamped placement,
open/close callbacks and installed-prefix accessibility coverage.

The additive `XmTimePicker` is now implemented and qualified with an explicit
wall-clock-only contract, ordered range validation, deterministic 24-hour or
12-hour presentation, field-wise keyboard editing, activation/value callbacks
and accessibility value semantics.  It does not infer a date, time zone, or
daylight-saving transition; callers resolve those when combining a value with
a date and zone.

**Complete (2026-10-25):** Calendar, DatePicker and TimePicker all pass their
installed-prefix fixtures and the existing additive-widget regressions remain
green.  Date-only and wall-clock values never silently acquire a host time
zone; daylight-saving gap/overlap resolution is therefore an explicit caller
responsibility rather than an implicit toolkit conversion.

### WP-27: DataGrid/TableView design and prototype

**Depends on:** WP-25 and WP-15.

**Objective:** make an evidence-backed go/no-go decision for the largest new
widget before committing to a public table model.

**Steps:**

1. Define a minimal data-model contract with stable row/column identities,
   ownership, column headers, sorting requests, selection, basic editing,
   value formatting and error reporting.  Explicitly exclude spreadsheet
   formulas, charts and a general application data-binding framework.
2. Prototype header/body geometry, resizing, scrolling, bounded viewport
   realization, focus/traversal, accessible table semantics and edits against
   an installed prefix without accessing private Xm records.
3. Establish benchmark corpora for small, typical and large bounded tables;
   measure memory, redraw, geometry and model-callback behavior under sorting,
   edits and mutation.
4. Record a design decision identifying any required model restrictions,
   performance limits and the accepted public C/resource/callback surface, or
   close the package no-go if the contract cannot be supported cleanly.

**Complete when:** the repository contains the prototype, benchmark evidence
and an explicit release decision; a no-go outcome is complete.

**Status (2026-10-25):** the public-API-only prototype and installed-prefix
fixture are implemented and pass 20 checks.  The bounded benchmark covers
small (64×8), typical (512×16), and maximum (4096×32) models, viewport scans,
selection/edit/sort operations, and deterministic model-memory measurements.
The documented decision is **go for WP-28**, with explicit restrictions on
bounded realization, stable IDs, request-only sorting, edit ownership and
accessibility providers; lazy providers, variable-height rows, frozen panes,
merged cells, formulas and general data binding remain excluded.

### WP-28: DataGrid/TableView release

**Depends on:** WP-27 with a go decision.

**Objective:** ship the accepted DataGrid/TableView as a separately additive
Motif widget class.

**Steps:**

1. Implement only the WP-27 model, selection, sorting-request and basic-edit
   contracts; keep model callbacks and ownership explicit.
2. Add public headers, resources, callback records, UIL/WML support where
   applicable, documentation and installed-prefix examples.
3. Test headers, columns, sorting requests, selection modes, editing commit/
   cancellation/error paths, scrolling, resize, model mutation, destruction/
   reentrancy, keyboard-only operation, accessibility and the documented
   bounded-table performance corpus.
4. Compare all existing `XmList`, `XmContainer`, `XmTree` and other baseline
   fixtures before and after the addition to prove no legacy behavior changed.

**Complete when:** the new grid meets its documented limits and installed
consumer suite, while it leaves all existing Motif widget APIs and components
unchanged.

**Status (2026-10-25):** the additive `XmDataGrid` is implemented with the
WP-27 bounded model, copied labels/cell values, stable opaque IDs, bounded
viewport realization, keyboard/wheel/top-row scrolling, request-only sorting,
explicit edit commit/cancel/error handling, table accessibility summary and an
installed-prefix consumer fixture.  The full additive regression set and the
WP-27 prototype/benchmark pass on `uconsole`; lazy providers, variable-height
rows, frozen panes, merged cells, formulas, charts and general data binding
remain explicitly excluded.

### WP-29: Modern-layout requirements corpus and contract decision

**Depends on:** WP-10 and the completed WP-11/WP-12 assessments.

**Objective:** determine, from new installed-client layouts rather than general
preference, whether a small additive layout family is justified after WP-12's
recorded no-go decision.

**Steps:**

1. Assemble representative new-application fixtures that expose avoidable
   nesting or attachment complexity in existing managers.  Keep the corpus
   independent of any private product or frontend.
2. Compare existing `XmForm`, `XmRowColumn`, `XmPanedWindow` and containers
   with proposed Stack, Flex/Flow and Grid contracts for geometry negotiation,
   resize stability, right-to-left direction, focus traversal and accessibility.
3. Define the smallest common public manager/constraint-resource vocabulary;
   reject CSS parsing, implicit layout mutation, global measurement caches and
   any contract that changes existing manager behavior.
4. Benchmark nesting depth, child mutation, resize storms, map/unmap and
   destruction/reentrancy.  Record a go/no-go decision independently for each
   candidate manager.

**Complete when:** the requirements corpus and decision record either justify
the individual layout packages below or retain the prior no-go result.  A
negative decision is a complete result.

**Status (2026-10-25):** the requirements corpus is documented from the
qualified WP-10/WP-11 installed consumer and its deterministic no-op control.
Stack, Flex/Flow, and Grid each have an independent **no-go** decision because
no reproducible consumer requirement exceeds the existing `XmForm`,
`XmRowColumn`, `XmPanedWindow`, and container composition contracts.  Reopen
criteria require geometry, resize, mutation, reentrancy, RTL, focus,
accessibility, and Xt-negotiation evidence from an installed consumer.

### WP-30: StackLayout

**Depends on:** WP-29 with a Stack go decision.

**Objective:** add a simple horizontal/vertical manager for everyday stacking
without changing `XmRowColumn` defaults or Form attachments.

**Steps:**

1. Define direction, gaps, outer padding, main/cross-axis alignment (including
   single-child centering), child visibility, directionality and deterministic
   handling of surplus and insufficient space.  Do not add a separate
   CentreLayout when these resources provide its complete contract.
2. Implement a new opt-in manager class with ordinary Xt geometry negotiation
   and public constraint resources; it must not inspect private child records.
3. Test one/many children, nested stacks, focus order, explicit child sizes,
   resize, child insertion/removal, right-to-left layout, accessibility and
   destruction from child callbacks against an installed prefix.

**Complete when:** StackLayout passes its installed geometry and lifecycle
suite while equivalent existing `XmRowColumn` fixtures remain unchanged.

**Status (2026-10-25):** deferred as **no-go**.  WP-29 found no installed
consumer requirement that exceeds existing `XmRowColumn` composition; reopen
only with the geometry, focus and accessibility evidence listed there.

### WP-31: FlexLayout and FlowLayout

**Depends on:** WP-30 and a Flex/Flow go decision from WP-29.

**Objective:** add a bounded modern flow manager with grow, shrink, alignment
and optional wrapping, without claiming browser-compatible CSS Flexbox.

**Steps:**

1. Specify direction, gaps, wrap, basis, grow/shrink factors, main/cross-axis
   alignment, baseline policy, rounding, minimum/maximum constraints and the
   explicit unsupported subset.
2. Implement only that contract as a new manager.  Child measurement and
   geometry requests must remain deterministic and must not cause unbounded
   relayout loops.
3. Test mixed fixed/flexible children, wrapping thresholds, constrained space,
   directionality, dynamic mutation, focus order, accessibility, resize storms
   and destruction/reentrancy from an installed prefix.

**Complete when:** FlexLayout has measured bounded geometry behavior and a
documented difference from CSS/browser layout; FlowLayout is a documented
configuration or is deferred rather than shipped as a duplicate class.

**Status (2026-10-25):** deferred as **no-go** with WP-29's independent
Flex/Flow decision.  No browser-style layout contract is introduced without a
new installed-client corpus and an explicit go decision.

### WP-32: GridLayout

**Depends on:** WP-30 and a Grid go decision from WP-29.

**Objective:** add an explicit rows-and-columns manager for dense forms and
dashboard-like layouts without altering `XmForm` attachment semantics.

**Steps:**

1. Define row/column tracks, gaps, spanning, alignment, fixed versus flexible
tracks, auto-placement policy, constraint validation and deterministic
conflict/error handling.
2. Implement a new manager with public child constraint resources and normal
Xt query/negotiation behavior; do not parse CSS grid syntax or infer state from
child names.
3. Test spans, empty tracks, explicit sizes, over-constrained layouts,
right-to-left direction, child mutation, focus order, accessibility, resize
storms and callback/destruction reentrancy against an installed prefix.

**Complete when:** GridLayout's constraints and failure behavior are documented
and its fixtures prove that existing Form geometry is unaffected.

**Status (2026-10-25):** deferred as **no-go** with WP-29's independent Grid
decision.  The toolkit retains existing Form, RowColumn and container
composition until a measured requirement justifies a new manager.

### WP-33: Overlay, Split and optional Dock layouts

**Depends on:** WP-29 with an individual go decision.

**Objective:** add only the secondary layout managers that the corpus justifies:
Overlay for z-order composition, a polished split manager, and optionally a
border/dock manager.

**Steps:**

1. Define independent scope and acceptance criteria for Overlay, Split and
Dock.  Overlay must define hit-testing, visibility and resize order; Split must
define pane constraints, keyboard access and divider behavior; Dock must define
edge/centre conflict policy.
2. Prefer a new Split manager over changing `XmPanedWindow`; compare it with
the existing widget and retain `XmPanedWindow` behavior exactly.
3. Implement only the managers with an evidence-backed need and test stacking,
divider dragging and keyboard operation where applicable, child mutation,
geometry negotiation, accessibility and destruction/reentrancy.

**Complete when:** each approved manager passes its own installed-prefix suite.
Unjustified managers remain deferred rather than becoming a broad layout
framework.

**Status (2026-10-25):** deferred as **no-go**.  WP-29 did not justify Overlay,
Split or Dock as a generic family; reopening requires an individual evidence-
backed contract and installed consumer fixture.

### WP-34: SegmentedControl

**Depends on:** WP-15.

**Objective:** add a compact mutually-exclusive segmented picker without
changing ToggleButton/radio-group appearance or behavior.

**Steps:**

1. Define item identity/ownership, selection/clear policy, labels/icons,
overflow, keyboard traversal, right-to-left order and accessible selected-item
semantics.
2. Implement a separate composite using public toggle/button facilities and a
shared resource-backed segment treatment; it must not modify a child's normal
toggle resources.
3. Test selection changes, disabled segments, mutation/removal, narrow widths,
scale/contrast, keyboard-only operation, callbacks and destruction reentrancy.

**Complete when:** the installed widget provides a documented segmented-picker
contract and all existing ToggleButton/radio fixtures retain their results.

**Status (2026-10-25):** implemented and qualified on the installed `uconsole`
prefix.  `XmSegmentedControl` owns a copied bounded item model and composes
ordinary public ToggleButton children, with label/icon support, disabled-item
filtering, single-selection/optional clear, RTL-aware keyboard traversal and a
deterministic clip-on-overflow policy.  The fixture passes 20 checks including
narrow geometry, accessibility summary, mutation and destruction from a
selection callback.  Existing ToggleButton behavior remains covered by the
legacy native-feature suite.

### WP-35: Compact content and adornment controls

**Depends on:** WP-15.

**Objective:** add small, composable controls for tags and presentation without
turning ordinary Labels, Separators or pixmap resources into new APIs.

**Steps:**

1. Define separate value/ownership/accessibility contracts for Chip/Tag/Token
with optional removal, Badge with bounded count/status and overflow text,
icon-and-text Label, text Divider and Avatar with image/initials fallback.
2. Implement each accepted control as an additive class or documented
composite.  Badge/Avatar placement must be explicit and must not introduce a
global overlay layer or silently alter a host button's geometry.
3. Test keyboard access to removable chips, non-color badge states, image-load
failure and initials fallback, label/divider localization, narrow layouts,
high-contrast/high-DPI output, accessibility and teardown.

**Complete when:** every shipped compact control has a bounded public contract,
and existing `XmLabel`, `XmSeparator` and pixmap behavior are unchanged.

**Status (2026-10-25):** the package is decomposed into independently
releasable contracts in `docs/compact-controls-contract.md`.  The additive
`XmChip` and `XmBadge` implementations and installed fixtures are complete and
documented in `docs/chip.md` and `docs/badge.md`.  The additive `XmIconLabel`
and `XmDivider` implementations and installed fixture are complete and
documented in `docs/icon-label-divider.md`.  The additive `XmAvatar`
implementation and installed fixture are complete and documented in
`docs/avatar.md`; all WP-35 controls now have explicit ownership,
accessibility and teardown gates.

### WP-36: Search, prompt text and Link controls

**Depends on:** WP-15 and WP-19.

**Objective:** add common text affordances while retaining ordinary TextField
editing, selection, validation and help behavior.

**Steps:**

1. Define a SearchBox with query-change versus committed-search callbacks,
clear-button behavior, icon ownership, accessible name/value and interaction
with IME/preedit text; it must not make blocking search requests itself.
2. Define prompt/placeholder text as an opt-in additive presentation that is
never submitted as user input, never replaces a real value and remains
distinguishable from entered text.
3. Define Link's label, target payload ownership, activation callback, visited
state policy, keyboard operation and non-color focus/visited treatment.  The
toolkit must not launch external programs or URLs on its own.
4. Test direct and keyboard operation, prompt transitions, clear/commit races,
IME boundaries in the supported cohort, scaling/contrast, accessibility,
callback reentrancy and destruction from an installed prefix.

**Complete when:** new SearchBox, prompt and Link APIs are documented and no
existing `XmTextField` or Label value semantics have changed.

**Status (2026-10-25):** the Link, SearchBox and prompt subpackages are
implemented and qualified on the installed `uconsole` prefix.  `XmLink` owns
copied label/target data, keeps dispatch in the application, supports
keyboard/pointer activation and a non-colour visited state, and exposes an
accessible action.  `XmSearchBox` subclasses the existing text field,
preserving its edit/IME path while adding query-changed, committed and clear
callbacks plus a presentation-only placeholder.  `XmPrompt` is a standalone
non-interactive presentation whose visibility never changes a sibling value.
Their fixtures pass 8, 11 and 9 checks respectively.

### WP-37: DisclosureGroup, FormSection and EmptyState

**Depends on:** WP-15 and WP-30 where the selected presentation needs
StackLayout.

**Objective:** add simple structural composites for collapsible settings and
empty views without changing `XmFrame`, `XmRowColumn` or application data views.

**Steps:**

1. Define DisclosureGroup expanded state, header/action ownership, keyboard
and accessible expanded semantics, and child map/unmap lifecycle.
2. Define FormSection header/footer, content ownership, spacing and narrow-
layout behavior; it is a presentation composite, not a form-data model.
3. Define EmptyState icon/message/action ownership and its relationship to the
application-owned view or model that determines emptiness.
4. Test expansion, focus restoration, dynamic child changes, header/footer
localization, optional actions, scale/contrast, accessibility and destruction/
reentrancy against an installed prefix.

**Complete when:** the composites are independently usable and have no effect
on existing Frame, RowColumn, List or Container behavior.

**Status (2026-10-25):** the existing `XmDisclosure` implementation already
meets the disclosure portion of this package and is documented in
`docs/disclosure.md`.  `XmFormSection` is now implemented as an additive
presentation manager with owned header/footer children, an application-owned
content host, copied `XmString` resources, bounded narrow-layout negotiation
and a panel accessibility projection; its installed fixture passes 11 checks.
`XmEmptyState` is now implemented as the matching additive presentation
manager with optional borrowed icon, copied message/action labels, explicit
action dispatch and bounded geometry; its installed fixture passes 11 checks.
The three WP-37 composites are independently usable and no existing
Frame/RowColumn/List/Container behavior is changed.

### WP-38: CommandPalette and Toast notification

**Depends on:** WP-19 and WP-23.

**Objective:** add bounded non-modal command discovery and transient
notifications without creating a navigation framework or a global message bus.

**Steps:**

1. Define CommandPalette command-model ownership, stable command IDs,
filtering, keyboard admission/dismissal, focus restoration, action dispatch
and accessible list semantics.  It may use AutoSuggestBox infrastructure but
must not execute commands from a drawing or filter callback implicitly.
2. Define Toast severity, optional action, queue/replace policy, timeout,
pause/dismiss behavior, screen-edge placement and accessible announcement.
3. Implement per-application-context instances with deterministic owner and
shutdown behavior; no process-global palette or notification queue is allowed.
4. Test rapid presentation/dismissal, focus, command/model mutation, timeout
teardown, accessibility, keyboard-only operation and callback reentrancy.

**Complete when:** installed fixtures prove that palette and toast lifetimes
are bounded and they do not alter dialogs, menus or application navigation.

**Status (2026-10-25):** `XmToast` is implemented as a per-instance,
application-placed composite with copied message/action strings, explicit
show/dismiss/pause semantics, bounded timeout ownership and accessible action
dispatch; its installed fixture passes 10 checks.  `XmCommandPalette` is an
additive, non-modal chooser with copied stable IDs/labels/shortcuts,
case-insensitive filtering, focus restoration, explicit application dispatch
and accessible panel/action semantics; its installed fixture passes 11 checks.
Neither control creates process-global state or alters dialogs, menus or
application navigation.

### WP-39: Loading, row-action and drag-feedback surfaces

**Depends on:** WP-17, WP-19 and WP-15.

**Objective:** add restrained modern feedback for loading and item operations
without inventing a generalized animation or gesture framework.

**Steps:**

1. Define Skeleton loading shapes, animation limits and reduced-motion/high-
contrast behavior.  Reuse WP-17's timer rules and provide a static fallback.
2. Define basic left/right row-action reveal for a new compatible list surface;
   it must use explicit pointer/keyboard actions and must not change `XmList`
   selection or scrolling semantics.
3. Define icon-and-shortcut context-menu entries and DropTarget drag highlight
   as opt-in presentations over existing menu/DnD contracts.  Do not change
   transfer ownership, acceptance or drop callbacks.
4. Test animation cancellation, constrained geometry, keyboard alternatives,
   pointer cancellation, DnD enter/leave/drop, high-contrast output,
   accessibility and teardown from an installed prefix.

**Complete when:** each accepted feedback surface has a non-animated or
keyboard equivalent and existing lists, menus and DnD behavior are unchanged.

**Status (2026-10-25):** `XmSkeleton` is implemented as the first bounded
loading surface.  It has an explicit static fallback, per-instance timer,
reduced-motion switch, copied accessibility label and deterministic start/
stop teardown; its installed fixture passes 7 checks.  `XmRowActions` is now
implemented as an opt-in sibling/overlay strip with copied IDs, labels and
shortcuts, explicit reveal/hide and keyboard equivalents, application-owned
dispatch, accessibility actions and destruction-safe child ownership.  Its
installed fixture passes 13 checks and confirms that an existing `XmList`
selection remains unchanged.  `XmDropHighlight` is now implemented as a
static, opt-in valid/invalid outline with a keyboard- and animation-free
state API and an accessible label; it never registers a drop site or changes
transfer ownership.  Its installed fixture passes 14 checks.  Context-menu
entries are closed as a documented **no-go**: existing public
`XmCreatePopupMenu`, `XmPushButton`, `XmNlabelPixmap`, `XmNaccelerator`,
`XmNacceleratorText` and `XmNactivateCallback` APIs already cover icon and
shortcut entries without a parallel abstraction.  The installed contract
fixture passes 9 checks; see `docs/context-menu-decision.md`.

### WP-40: Bounded application-chrome composites

**Depends on:** WP-15, WP-19 and relevant layout-manager go decisions.

**Objective:** add opt-in client-area composites for StatusBar, toolbar
overflow and collapsible sidebars without taking ownership of application
navigation or window-manager title bars.

**Steps:**

1. Define StatusBar item ownership, priority/overflow, progress/indicator
presentation and accessible status announcement.  It is not a replacement for
application-specific status models.
2. Define a toolbar composite that moves explicitly marked overflowable actions
into a normal menu at constrained widths while preserving action identity,
shortcuts, focus and callback behavior.
3. Define a collapsible-sidebar composite with explicit content ownership,
expanded width, focus restoration and keyboard toggle.  It must not introduce
an adaptive navigation stack or navigation policy.
4. Evaluate a header-bar client-area presentation separately.  Do not change
window-manager title-bar controls, decorations or system integration unless a
portable supported contract and real-window evidence exist; defer it otherwise.
5. Test resizing/overflow, dynamic action/item mutation, keyboard-only use,
right-to-left layout, accessibility, owner destruction and installed-prefix
visual fixtures.

**Complete when:** each shipped composite has a narrow client-area contract;
legacy MainWindow, menu, toolbar and shell behavior remain unchanged.

**Status (2026-10-25):** `XmStatusBar` is implemented as the first bounded
application-chrome composite.  It copies item IDs and labels, renders text or
progress indicators, keeps higher-priority items visible under constrained
width, reports overflow count, exposes a named accessibility panel and tears
down its owned children safely.  Its installed fixture and toolbar/sidebar
work remain separate packages.  The installed fixture passes 13 checks.
`XmToolbar` is implemented as the second package: it copies action IDs,
labels and shortcut text, keeps explicitly non-overflowable actions in the
strip, moves lower-priority overflowable actions into an ordinary Motif menu,
preserves application dispatch and exposes a named toolbar panel.  Its
installed fixture passes 13 checks; collapsible-sidebar work remains separate.
`XmSidebar` is implemented as the bounded toggle/content composite with
explicit content host ownership, expanded/collapsed widths, Ctrl+M keyboard
toggle and restoration of focus into content after expansion.  Its installed
fixture passes 13 checks; adaptive navigation remains application-owned.
The header-bar evaluation is closed as a documented **defer**: ordinary Xt/X11
does not define portable ownership of window-manager decorations or title-bar
input.  See `docs/header-bar-decision.md`; reopening requires a versioned
server/desktop contract and real-window evidence.

Appearance and scalable assets are intentionally the first delivery track: they
produce user-visible improvement while preserving the established programming
model.  The additive-widget track may progress independently where its stated
dependency lines allow it.

### WP-41: Cross-cutting allocation and ownership hardening

**Depends on:** the affected public API's existing regression coverage.

**Objective:** remove concrete arithmetic, allocation-failure and ownership
hazards exposed by maintenance review and the peer bug-fix stream without
changing the Motif/Xt programming model or hiding failures behind fallbacks.

**Steps:**

1. Select one bounded defect at a time and identify the existing ownership,
   locking and ABI contract before editing it.
2. Make the operation transactional where failure could otherwise publish a
   partial object, array, table or widget state. Preserve public handle and
   resource identity when the existing contract requires it.
3. Add a source-contract regression for arithmetic and cleanup paths, plus an
   installed native fixture whenever the state can be exercised through the
   public API.
4. Record the supported configuration, failure result, dependency boundary and
   exact validation evidence in `CHANGES` and the relevant documentation.
5. Integrate peer fixes only after checking their dependencies against this
   fork; do not copy a later-branch label, test assumption or ownership model
   that is not present here.

**Complete when:** each selected defect has a reviewed implementation, focused
and installed-library verification, and documented ownership behavior. A peer
backlog item that depends on a broader refactor remains explicitly deferred
rather than being represented as complete.

**Status (2026-10-25):** the first render-table slice is implemented and
qualified. `XmRenderTableCopy` now checks replacement allocations and rolls
back copied entries; `XmRenderTableAddRenditions` rejects unrepresentable
allocation sizes; `XmRenderTableRemoveRenditions` plans
retained entries and publishes unique-table compaction or shared-table
detachment only after allocation and rendition-copy success. Source checks and
the installed Xft `font-lifetime` fixture cover count guards, unique and shared
removal, ownership retention and empty-table behavior. The full libXm rebuild
and gallery integration pass on `uconsole`. The unmatched-rendition append path
also rolls back replacement bodies and copied handles before returning the
source table on allocation or copy failure. Shared-table merge-copy rollback is
also covered in this slice. The reviewed peer render-table sequence through
copy-size, count, compaction, null-extents, removal-copy, append, merge,
creation, copy-locking, rendition-lookup-locking, and display-less-locking
guards is now integrated as separate commits; no peer patch was copied without
checking its ownership and locking dependencies in this fork.
Merge replacement and copy-on-write clone failures now preserve their
existing rendition as well. Rendition helper and initial-table allocation
failures are guarded and rollback-safe; the exact Xft `font-lifetime` fixture
and the no-Xft gallery integration both pass on `uconsole`.
ToggleButton shadow, fill, and etched-menu paths now reject undersized geometry
before unsigned `Dimension` subtraction and skip unavailable shadow GCs; the
source contract and fresh library/gallery qualification pass on `uconsole`.
`XmFontListCreate`, `XmFontListCreate_r`, `XmFontListAdd`,
`XmFontListEntryCreate`,
`XmFontListEntryCreate_r`,
`XmFontListEntryLoad`, and
`XmFontListAppendEntry`, `XmFontListEntryGetFont`,
`XmFontListEntryGetTag`, `XmeRenderTableGetDefaultFont`,
`XmFontListGetNextFont`, and `XmFontListRemoveEntry` no longer hold
process/application locks while
entering locking charset, rendition, and render-table helpers; the source
contract and both no-Xft/Xft installed-library fixtures pass on `uconsole`.
`_XmRenditionCopy` now rejects tag-count multiplication that cannot be
represented by `size_t` or Xt's `Cardinal` allocator width before copying;
the source contract and both installed-library build variants pass on
`uconsole`.
Render-table property token allocations now terminate through the existing EOF
parser path on allocation failure; the source contract and both installed
library build variants pass on `uconsole`.
Quoted and alphabetic token lengths are also checked against Xt's `Cardinal`
width before allocation.
Cloned rendition scratch-tag copies now release the partial rendition and
handle when their tag array cannot be allocated.
Display-less `XmRenderTableGetTags` and `XmRenderTableGetRendition` calls now
use the process lock instead of deriving an application context from a null
display.
`XmRenderTableGetRenditions` follows the same display-less process-lock
contract. Its allocation-failure cleanup uses the internal rendition and
handle release helpers while the lock is held, avoiding recursive public
locking without changing the returned-array ownership contract. The installed
source test is `tests/automated/check-render-table-rendition-array.py`.
`XmStringLtoRCreate` likewise no longer holds the process lock while entering
direction, concatenation and separator constructors; its compatibility
ownership contract is protected by
`tests/automated/check-xmstring-obsolete-locking.py` and
`docs/xmstring-obsolete-locking.md`.
`XmStringGenerate` now follows the same lock delegation rule across its parse,
cache, component and concatenation helpers; the source contract is recorded in
`tests/automated/check-xmstring-generate-locking.py` and
`docs/xmstring-generate-locking.md`.
`XmCvtByteStreamToXmString` now delegates synchronization to its cache and
representation helpers rather than holding the process lock across ASN.1
conversion; the source contract is recorded in
`tests/automated/check-xmstring-byte-conversion-locking.py` and
`docs/xmstring-byte-conversion-locking.md`.
`XmStringTableProposeTablist` now delegates synchronization to tab-list,
conversion, and rendering helpers rather than holding the process lock across
them; the source contract is recorded in
`tests/automated/check-xmstring-tablist-locking.py` and
`docs/xmstring-tablist-locking.md`.
`XmStringHasSubstring` and `XmeGetNextCharacter` now delegate synchronization
to their string and component helpers rather than holding the process lock
across substring scans and parse callback creation; the source contract is
recorded in `tests/automated/check-xmstring-function-locking.py` and
`docs/xmstring-function-locking.md`.
`XmStringToXmStringTable` now delegates synchronization to its context and
string helpers, and frees its active context if table allocation fails; the
source contract is recorded in
`tests/automated/check-xmstring-table-locking.py` and
`docs/xmstring-table-locking.md`.
`XmStringIsVoid` and `XmCvtXmStringToByteStream` now delegate synchronization
to `XmeStringGetComponent` rather than holding the process lock across repeated
component reads; the source contract is recorded in
`tests/automated/check-xmstring-enumeration-locking.py` and
`docs/xmstring-enumeration-locking.md`.
`XmStringGetNextComponent` now delegates synchronization to
`XmeStringGetComponent` rather than holding the process lock across component
extraction; the source contract is recorded in
`tests/automated/check-xmstring-getcomponent-locking.py` and
`docs/xmstring-getcomponent-locking.md`.
`_XmStringGetSegment` now validates the context and required output pointers
before initializing the compatibility direction value; the source contract is
recorded in `tests/automated/check-xmstring-segment-validation.py` and
`docs/xmstring-segment-validation.md`.
Context enumeration now checks copied rendition and tag strings before taking
their length, returning end-of-context and updating the error flag on failure;
the source contract is recorded in
`tests/automated/check-xmstring-context-copy-failure.py` and
`docs/xmstring-context-copy-failure.md`.
`_XmStringSingleSegment` now validates both output pointers before clearing or
writing them; the source contract is recorded in
`tests/automated/check-xmstring-single-segment-outputs.py` and
`docs/xmstring-single-segment-outputs.md`.
`XmStringGetNextComponent` and `XmStringGetLtoR` now reject missing required
output pointers before advancing or initializing extraction state; the source
contract is recorded in `tests/automated/check-xmstring-get-outputs.py` and
`docs/xmstring-get-output-validation.md`.
`XmStringGetLtoR` now delegates locking to its context helpers and checks
context initialization before enumeration, with the source contract recorded
in `tests/automated/check-xmstring-getltoR-locking.py` and
`docs/xmstring-getltr-locking.md`.
Non-optimized ASN.1 conversion now commits rendition counts only after growth
succeeds, checks separator line allocations, and cleans partial state on
failure. The source contract is recorded in
`tests/automated/check-xmstring-asn1-failure-cleanup.py` and
`docs/xmstring-asn1-failure-cleanup.md`.
Rendition begin/end setters now commit resized tag arrays and counts only after
`XtRealloc` succeeds, preserving prior formatting state on allocation failure.
The source contract is recorded in
`tests/automated/check-xmstring-rendition-resize.py` and
`docs/xmstring-rendition-resize.md`.
Internal unoptimization and segment insertion now check array-entry
allocations before initialization, preserving source and copied ownership on
failure. The source contract is recorded in
`tests/automated/check-xmstring-entry-allocation.py` and
`docs/xmstring-entry-allocation.md`.
Internal XmString conversion constructors now check representation allocations
before initialization, and segment extraction reports failed construction
through its context error state. The source contract is recorded in
`tests/automated/check-xmstring-conversion-failures.py` and
`docs/xmstring-conversion-failures.md`.
Cached tab/empty constructors, cloning, and component conversion now check
representation allocation before use and return failure without copying into
missing storage. The source contract is recorded in
`tests/automated/check-xmstring-component-allocation.py` and
`docs/xmstring-component-allocation.md`.
Cached direction/separator constructors now validate representation allocation
and release the process lock before rollover cleanup and recursive recreation.
The source contract is recorded in
`tests/automated/check-xmstring-cached-create.py` and
`docs/xmstring-cached-create.md`.
`_XmStringNCreate` now checks optimized and non-optimized representation
allocations before initialization and returns `NULL` cleanly on failure. The
source contract is recorded in
`tests/automated/check-xmstring-create-failure.py` and
`docs/xmstring-create-failure.md`.
XmString table parse and unparse now clean already-converted elements and the
output table on conversion failure. The source contract is recorded in
`tests/automated/check-xmstring-table-transaction.py` and
`docs/xmstring-table-transaction.md`.
`_XmTabListGetPosition` now rejects a missing `Screen` before unit conversion,
preserving the zero-position fallback. The source contract is recorded in
`tests/automated/check-tab-position-screen-guard.py` and
`docs/tab-position-screen-guard.md`.
VendorS secondary-object creation now reads the class size under one process
lock and performs allocations after unlocking, avoiding recursive acquisition
and mismatched release. The source contract is recorded in
`tests/automated/check-vendors-lock-balance.py` and
`docs/vendors-lock-balance.md`.
XmTabList now uses explicit unlocked copy/free helpers internally, with public
copy/free APIs retaining the process-lock boundary. The source contract is
recorded in `tests/automated/check-tablist-lock-boundaries.py` and
`docs/tablist-lock-boundaries.md`.
Tab-list replacement and removal now validate every supplied position against
the copied list and discard invalid private state without mutating the caller's
list. The source contract is recorded in
`tests/automated/check-tab-position-bounds.py` and
`docs/tab-position-bounds.md`.
The optional XLibre resolver also rejects non-positive or unrepresentable
RandR output counts before allocating its output table, preserving the global
DPI fallback for malformed server metadata. The guard is covered by the
installed source contract in `tests/automated/check-xlibre-lifetime.py`.

## Acceptance criteria for every increment

Before a modernization increment is accepted:

1. Keep old public behavior intact unless the change documents a narrowly
   scoped bug correction or newly selected opt-in feature.
2. Build and install a fresh, isolated toolkit prefix, then build its consumers
   against that prefix and prove the runtime-loaded libraries originate there.
3. Add focused automated tests for normal operation, invalid input, resource
   precedence, resizing, keyboard-only use, lifecycle/destruction and
   callback reentrancy as applicable.
4. For visual work, inspect the actual window from the verified built process;
   retain concise image/geometry evidence instead of relying on a standalone
   renderer.
5. For accessibility work, exercise both native semantic behavior and the
   optional external transport/client path where claimed.
6. Record source and asset fingerprints, configuration, start/end/elapsed time,
   output path and platform limits.  Investigate anomalous durations rather
   than treating a fast command as proof of freshness.
7. Update `CHANGES` and the relevant documentation with both guarantees and
   known limits.  Do not describe untested desktop, architecture, input-method,
   screen-reader or sanitizer scenarios as supported.

## Near-term exclusions

The following are deliberately not part of this plan: a GTK/Qt port, a new
object system inside `libXm`, removal of Xt resource files or UIL, transparent
thread-safe widget access, a replacement rendering engine, or automatic
per-monitor scaling on an X server without a tested scale contract.  XLibre's
versioned RandR `DPI` property is the explicit exception covered by WP-04A.
Those other cases would be separate products or rewrites, not compatible
modernization work.

The additive-widget track also excludes media playback, ink canvases/toolbars,
browser embedding, charts, gauges, barcodes, a general-purpose virtualization
engine and adaptive application-navigation chrome.  They have different
rendering, lifecycle or application-level contracts; the presence of a new
Motif widget set is not a reason to add them to this toolkit.
