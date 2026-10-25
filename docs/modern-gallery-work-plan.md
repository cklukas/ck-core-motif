# WP-05: live modern component gallery

## Objective and completion rule

Build and ship one maintained native demonstration application, tentatively
named `ckmotif-gallery`, that is both a polished component gallery and an
executable reference for every constructible CK Core Motif GUI component
carried by this source tree.
Its information architecture is inspired by contemporary desktop component
galleries: a collapsible categorized navigation pane, a responsive content
area, and paired **Example** and **Source** tabs.  It must use the installed
toolkit interfaces it demonstrates; private widget records and demo-only
workarounds are not acceptable.

The gallery is complete only when all of the following are true:

- every constructible visual widget class, legacy/demo extension, and additive
  CK Core Motif component is represented by a live, categorized example;
- every advertised interaction is keyboard- and pointer-operable, exposes the
  correct accessibility state/action, and has automated native coverage;
- switching appearance profiles and simulated desktop DPI changes a realized
  gallery immediately, without losing focus, values, selection, expansion,
  scroll position, callbacks, or explicit application resources;
- a real XLibre multi-output move changes a realized shell's effective scale
  and reflows the gallery through the same toolkit scaling transaction;
- each example has a matching source explanation derived from the code that
  constructs it, so the display and documentation cannot drift; and
- ckdocs contains current, attributable screenshots and complete component
  reference material.  Rendered documentation is built and inspected once,
  from the primary checkout, at feature completion.

"All components" means every constructible visual widget class in the Motif
and CK Core Motif sources, including installed headers, legacy/demo extension
headers, and the fork's additive widgets.  Non-visual protocol types and truly
private implementation classes are excluded only with an explicit inventory
record.  A legacy/demo widget is not a reason to omit it: this work either
graduates it to a documented, tested gallery component or repairs the toolkit
until it can be done.  The inventory below records each disposition explicitly.

## Current evidence and gap statement

The existing `modern-light`, `modern-dark`, and `high-contrast` profiles are
creation-time X resource fallbacks.  Their documented contract deliberately
makes a profile immutable after selection.  `Xft.dpi` likewise supplies a
global default scale and does not promise live reflow.  The optional XLibre
adapter can observe and select a shell's output DPI, but the currently recorded
evidence does not qualify a real multi-output property change, move, hotplug,
spanning-window reflow, or destruction while a reflow is queued.

Those limitations are blockers for this work package, not conditions the
gallery may conceal.  The implementation must extend the toolkit contracts and
tests before presenting live switches as working features.

## Work packages

### [x] WP-05.0 — Baseline, inventory, and acceptance manifest

- Inventory all installed and source-carried component headers, widget classes,
  legacy/demo extensions, additive components, dialogs, managers, gadgets, and
  non-visual exclusions.  Give every entry a stable identifier, category,
  construction owner, source fragment, automated interaction scenario,
  documentation anchor, screenshot scenario, and graduation/rework status.
- Publish the machine-readable manifest with a checked-in human-readable table.
  A verification tool must fail if any constructible component lacks a
  disposition or if an example/documentation/source fragment points to a
  missing entry.  It must also reject a legacy/demo extension that is claimed
  as complete without its promotion/repair evidence.
- Establish an isolated, reproducible native build/installation prefix and a
  gallery test environment under the approved temporary volume.  Record source
  and asset hashes, build inputs, command, timing, output paths, and runtime
  library identity.  The first gallery increment records the inventory and
  verifier contract; the native gallery prefix and visual receipts remain
  deliverables of the shell packages below.
- Inspect the existing appearance, DPI, XLibre, rendering, accessibility, and
  demo code paths.  Convert every discovered toolkit defect into a focused
  regression before or alongside its correction.

### [ ] WP-05.1 — Live appearance transaction in the toolkit

- Replace the creation-only profile limitation with a documented public live
  appearance operation that applies `classic`, `modern-light`, `modern-dark`,
  and `high-contrast` to an already-realized display/widget hierarchy.
- Model profile-owned defaults separately from explicit application values and
  ordinary application X resources.  A live change must update only values the
  selected profile owns; explicit resources retain their precedence and exact
  values.
- Make palette, focus indicators, typography/spacing defaults, images that are
  documented as scale/theme-derived, and geometry invalidation participate in
  one UI-thread transaction.  Repaint and relayout only after the transaction
  is internally consistent.
- Preserve widget identity, child ownership, callbacks, traversal focus,
  selections, editing state, timers, and accessibility object identity across
  each change.  Define and test failure/rollback behavior for allocation,
  converter, and callback reentrancy failures.
- Add installed-consumer regressions covering every profile transition,
  precedence, keyboard traversal, accessibility colors/states, destruction
  during a change, and repeated changes.  Update the appearance API reference
  and `CHANGES` with the resulting public contract.

### [ ] WP-05.2 — Live DPI transaction and deterministic simulation API

- Define one canonical logical scale contract that reconciles the historical
  `Xft.dpi / 75` defaults with XLibre's `DPI / 96` output hint.  Document the
  conversion, baseline, rounding/saturation, fallback, and explicit-resource
  rules rather than letting each widget interpret DPI independently.
- Add a public, testable display-scale override intended for developer tools
  and the gallery's 96, 144, 192, and 288 DPI buttons.  It must follow the same
  transaction path as the external-output backend, be reversible, and never
  mutate a caller's X resource database as a side effect.
- Audit all widget default dimensions, fonts, gaps, borders, image/pixmap
  handling, preferred sizes, and geometry managers.  Move each scale-derived
  default to the common transaction; retain explicit pixel resources unchanged
  unless their existing public contract declares them scale-relative.
- Recompute layout and repaint live while preserving values, selection, focus,
  scroll offsets where representable, and callback wiring.  Cover nested
  managers, dialogs, notebooks, menus, text widgets, gadgets, and all additive
  components.
- Add installed native regressions for each simulated scale, transitions in
  both directions, repeated changes, resource precedence, saturation, focus,
  text editing, accessibility bounds, and destruction/reentrancy during
  reflow.  Correct defects in the owning toolkit layer, with a regression for
  each fix.

### [ ] WP-05.3 — Complete the real XLibre multi-output path

- Wire the existing optional XLibre shell observer to the same DPI transaction
  used by the simulation API; do not keep a gallery-specific monitor-move path.
- On a real multi-output XLibre server, exercise moves between outputs with
  distinct `DPI` properties, runtime property changes, output add/remove,
  center-in-gap selection, a spanning shell, and a queued change whose callback
  destroys the shell.  Verify the window and descendants, not merely internal
  helper return values.
- Capture observer registration, effective output/DPI/scale, transaction
  ordering, final geometry, source and installed-library fingerprints, and
  screenshots in the qualification receipt.  Keep ordinary-Xvfb fallback
  coverage distinct from real-XLibre qualification.
- If XLibre exposes an ambiguity or toolkit lifetime/geometry bug, repair the
  shared implementation and extend the native regression suite before the
  gallery claims the scenario works.

### [ ] WP-05.4 — Gallery application architecture and shell

- Add one maintained demo target under `demos/programs/` with a normal
  autotools build/install path, strict warnings, and no dependence on private
  headers.  Its target and documentation are part of the distribution.
- Build a native application shell with an accessible header, theme selector,
  DPI selector, effective-scale/status readout, and a collapsible sidebar.
  The sidebar groups the manifest categories (layout, commands/navigation,
  buttons and selection, text/input, data views, feedback/dialogs, media, and
  additive components) and restores focus predictably when collapsed.
- Present the selected component in a responsive content host with **Example**
  and **Source** tabs.  The Example tab contains live controls and an
  interaction/status log.  The Source tab presents the exact construction,
  resource, callback, and teardown fragments used by that example, with
  explanatory notes and links to the API documentation.
- Keep construction code as the single source of truth: metadata may select
  annotated source fragments, but no hand-copied strings are allowed to become
  the code tab.  Validate the fragment extraction in CI/automated tests.
- Implement pointer, keyboard, touch/drag where applicable, resize, sidebar
  collapse, category navigation, source-tab navigation, profile switching, and
  DPI switching as native interactions.  Do not simulate a component with a
  static image or an inert mock.

### [ ] WP-05.5 — Component examples and interaction coverage

- Add a concise but complete example for each manifest entry.  Examples must
  demonstrate construction, key resources, state changes, callbacks, cleanup,
  and keyboard behavior; stateful components must show realistic mutation as
  well as initial rendering.
- Exercise dialogs, menu/popup lifecycles, drag-and-drop, scrolling, selection,
  edit/validation, data navigation, canvas/provider accessibility, and
  component-specific edge cases through their real public APIs.
- Add native interaction tests driven through the X server where appropriate.
  They must verify visible state, callback count/order, accessibility state and
  bounds, focus transitions, and resource preservation through theme/DPI
  changes.  Test two independent instances and callback-triggered destruction
  for reusable components.
- Treat every mismatch found while constructing an example as a product defect:
  fix the library contract/implementation, add the focused regression, update
  the example, and document the corrected behavior.

### [ ] WP-05.6 — ckdocs reference, screenshots, and provenance

- Add a gallery guide, a complete component catalogue, and focused pages for
  every additive component not already fully documented.  Each entry includes
  purpose, public construction code, resources, callbacks, interaction and
  keyboard behavior, accessibility contract, live theme/DPI behavior, limits,
  and a link to its gallery example.
- Generate screenshots from the verified gallery binary for the catalog landing
  view, representative category pages, all four appearances, and each
  simulated DPI.  Add real-XLibre screenshots only to the validated scenario;
  label server/output conditions and do not pass simulated images off as a
  physical monitor move.
- Store durable image assets in `docs/images/`, with a manifest that records
  exact binary/source hashes, effective profile/DPI, viewport, capture command,
  and expected dimensions.  An automated provenance check rejects stale,
  missing, or mislabeled captures.
- Update navigation and cross-links.  Once the feature is complete, build
  rendered ckdocs exactly once from the primary checkout and verify the output
  contains the gallery anchors, component entries, and image references.

### [ ] WP-05.7 — Qualification, publication, and maintenance handoff

- Run the complete relevant installed native suite, new gallery interaction
  suite, accessibility checks, profile/DPI transitions, source-fragment and
  screenshot provenance checks, and real-XLibre matrix.  Preserve concise
  receipts with commands, results, elapsed time, output identity, and known
  environmental boundaries.
- Inspect the actual gallery window from the verified newly built executable at
  every appearance and DPI level, including a true XLibre output move.  Compare
  it to the captured documentation assets before declaring visual completion.
- Update `CHANGES`, public documentation, distribution files, and test
  instructions.  Review all public names, comments, documentation, tests,
  screenshots, and commit messages for self-contained open-source rationale.
- Commit only the finished, passing toolkit/demo/documentation change and
  publish it to the repository's configured `ckgit` and `origin` targets as
  required by this repository's accepted-fix policy.  Verify both remote branch
  tips contain that commit; do not force-push.

## Evidence tracker

Update this table as work completes.  A checked box without the corresponding
evidence location is not completion.

| Work package | Status | Evidence / notes |
| --- | --- | --- |
| WP-05.0 Baseline and manifest | Complete | [`modern-gallery-manifest.json`](modern-gallery-manifest.json) records 94 public class declarations (85 constructible, 9 explicit exclusions); `tests/automated/check-gallery-manifest.py` verifies exact coverage against `lib/Xm/XmAll.h`. Native gallery build and visual receipts remain later-package gates. |
| WP-05.1 Live appearance | In progress | The additive `XmApplyAppearanceProfile` transaction now covers registered core widgets and cache-backed gadgets, preserves explicit/application resources and widget state, rejects nested profile swaps, and fails atomically before commit. Commits `9de24980`, `b285de0c` and `20f5c906` are qualified by repeated live, rollback, gadget, profile and consumer ASan/UBSan fixtures. The complete historical-widget matrix remains open. |
| WP-05.2 Live DPI simulation | In progress | The additive `XmSetDisplayScaleOverride` API drives a shared UI-thread transaction covering ToggleButton, SpinBox, ProgressRing and automatic ScrolledWindow defaults, with explicit-resource preservation, reversible rollback and the 96/144/192/288 matrix qualified by installed AArch64 fixtures. Commits `0d72d56d` and the earlier scale cohort provide the evidence; full historical-widget coverage and real XLibre output qualification remain open. |
| WP-05.3 Real XLibre | Planned | Existing observer has no recorded real multi-output qualification. |
| WP-05.4 Gallery shell | Planned | No unified maintained gallery target exists. |
| WP-05.5 Examples/tests | Planned | Existing demos are separate historical programs. |
| WP-05.6 ckdocs/screenshots | Planned | No gallery screenshot/provenance set exists. |
| WP-05.7 Qualification/publication | Planned | Starts only after all preceding packages pass. |

## Sequencing and change discipline

WP-05.0 establishes the complete inventory and baseline.  WP-05.1 and
WP-05.2 are foundational and may proceed in small tested changes, but the
gallery may consume only their finished public contracts.  WP-05.3 validates
the external backend against WP-05.2; WP-05.4 and WP-05.5 consume both live
transactions.  WP-05.6 uses only screenshots from the verified gallery binary,
and WP-05.7 is the final quality gate.

Every build, generated capture, cache, and scratch artifact for this work is
created in a unique directory under the approved temporary volume.  Superseded
builds are removed after their concise receipt is retained.  The primary source
checkout remains the sole location for the final documentation build.
