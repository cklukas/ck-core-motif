# CK Core Motif modernization plan

## Purpose

Modernize CK Core Motif as a maintained native Xt toolkit without replacing its
widget model, resource database, callback contracts, UIL support, or existing
applications.  The target is a more capable and pleasant toolkit for new code
while retaining the APIs and behavior that existing Motif clients deliberately
use.

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

If common application layouts remain unnecessarily difficult after that work,
add a new opt-in layout-manager widget with a small, documented grid/flex-like
contract.  It is preferable to changing `XmForm` behavior.  The new manager
must support keyboard focus order, resizing, internationalized direction,
accessibility, geometry negotiation and destruction/reentrancy tests from its
first release.

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

**Status (2026-10-24):** implemented and qualified on the AArch64 `uconsole`
Linux host with Xvfb.  Fresh installed-prefix runs passed at Xft.dpi 96 and
144; each receipt records the fixture/library/result/image hashes.  The full
source-tree build also exposed and allowed correction of a compile error in the
preceding bounded text-selection change.  The WML generator remains outside
this baseline, but its fresh out-of-tree build now passes after making lexer
and parser generation explicit.

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

**Status (2026-10-24):** phase 1 is implemented and documented for the
qualified AArch64 Linux cohort. The installed native fixtures cover scalar and
grapheme movement, the maintained mixed-script/fallback corpus, local Compose,
real IBus XIM commit/cancellation and focused provider restart/loss behavior,
plus UTF-8 clipboard round trips. `docs/native-text-ime-matrix.md` records
the exact evidence boundary and keeps x86_64, broader provider/engine
interoperability, candidate panels and sanitizer exit ownership as explicit
open gates. The matrix does not treat unavailable services or interposed
callbacks as provider passes.

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

**Status (2026-10-24):** phase 1 is implemented and qualified on the native
AArch64 installed cohort. Native/provider fixtures cover roles, names, states,
actions, selection, text events, modality and destruction/reentrancy for the
listed surfaces, with focused AT-SPI observers and a bounded Orca/eSpeak
baseline. `docs/accessibility-journey-matrix.md` records the distinction
between those protocol results and real-client usability. Complete x86_64,
all desktop screen-readers, application journeys and clean sanitizer exit
ownership remain explicit gates.

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

**Status (2026-10-24):** the public design proposal and explicit decision are
recorded in `docs/cpp-support-library-proposal.md`. The consumer-only prototype
has a GO decision; an installed WP-09 release remains NO-GO until its callback
exception barrier, widget-destruction ordering and mixed C/C++ ownership
fixture pass against a fresh prefix.

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

**Status (2026-10-24):** no-go.  WP-08 has a reviewed consumer-only design,
but its callback exception-barrier, destruction-order and mixed ownership
fixture has not yet passed against a fresh installed prefix.  No partial C++
SDK is installed or required by `libXm`; see
`docs/cpp-support-library-release-decision.md` for the release gate and reopen
criteria.

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

Appearance and scalable assets are intentionally the first delivery track: they
produce user-visible improvement while preserving the established programming
model.  The other packages may progress independently where their dependency
lines allow it.

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

### WP-14A: Transfer, image and storage lifetime maintenance

Depends on WP-00. Keep the public Motif resource and callback APIs unchanged.

1. Audit native X property assembly and conversion-buffer ownership at format
   8/16/32 boundaries and allocation limits.
2. Bound XPM memory/file token parsing, decimal fields, and image file I/O;
   preserve valid historical syntax and pixel formats.
3. Make repeated selection ownership and callback teardown idempotent.
4. Run the isolated Linux regression runner against one installed prefix and
   record source/library fingerprints with each result.

Status (2026-10-25): transfer clipboard, merge/bounds, ownership lifetime,
XPM, and Xt callback fixtures pass on uconsole; clipboard/merge/bounds/lifetime
use strict non-sanitized runs because the external Xt selection manager retains
display-owned records until process teardown, while XPM and Xt callback runs
also pass ASan/UBSan consumer checks. The separate scroll-frame reentrant
destruction case remains assigned to the widget-lifetime follow-up.
