# Modern visual overhaul: implementation and acceptance

The implemented modern appearance cohort is being consolidated with the
maintenance branch. [Integration qualification](modern-integration-qualification.md)
records the final build and test scope. The broader design plan below remains
a roadmap; integrating this cohort does not claim every planned matrix is complete.

## Agreed visual contract

Modern light and dark use refined, balanced controls with restrained depth,
readable typography and visible affordances. Control shapes may be redesigned;
classic rendering remains available. Scrollbars retain refined end arrows and
reserved layout space, rather than becoming minimalist overlays.

Comfortable and compact densities use minimum control heights of 34 and 28
logical pixels, indicator sizes of 18 and 16, scrollbar thicknesses of 18 and
16, and a shared 4/8/12/16/24 spacing scale. Geometry grows to fit larger fonts.
The default body font is sans-serif 11 pt, titles 18 pt, section headings 12 pt
bold, and code monospace 11 pt. Focus uses a distinct two-pixel outline.
Application resource overrides remain authoritative.

## Original delivery ledger (historical)

| Work package | Required result | Status |
| --- | --- | --- |
| Shared appearance | One definition for startup and live styling; explicit resource ownership | Native primary/secondary parity, wildcard precedence and classic restoration accepted for representative classes |
| Density and roles | Public set/get/apply density; normal/title/section/secondary/code/primary/destructive roles; public spacing query; UIL registration | Density API and spacing accepted; ABI 9 role resources, live updates and UIL implemented in the working branch; full qualification pending |
| Typography and geometry | Measured render tables throughout; DPI applied once; natural sizes and live updates | Shared fonts and 96-DPI live scaling implemented with native parity/rollback coverage; composite ownership and natural geometry pending |
| Toggles and buttons | Strong check/radio/mixed states; 44x24 switch; connected segmented groups; coherent buttons | Shared symmetric faces and connected focus outlines implemented; native state/interaction checkpoints recorded below; full accessibility and application matrix still open |
| Scrollbars and tracks | Refined arrow buttons, thumb, track, slider, scale and progress treatment | Shared chevrons, tracks, sizing, wheel behavior and ring raster implemented with native checkpoints; final cross-application acceptance still open |
| Editing and navigation | Consistent fields, dropdowns, spin controls, tabs, menus, toolbars, lists, trees and grids | Composite frames/arrows, tree connections, notebook flaps and tab scrolling implemented; long-label readability, per-node accessibility and remaining lifecycle cases still open |
| Dialogs and glyphs | Toolkit-owned alignment/wrapping/actions; complete glyph family and non-SVG fallback | SVG glyph family and dialog spacing implemented and tested; all reachable dialogs still require the final visual matrix |
| Gallery simplification | No custom modern fonts/colors/borders/renderers or natural-size repairs; use standard semantic roles | Toolkit styling, semantic roles, focused Source tabs and live controls implemented; explicit dimensions remain in 32 construction blocks and content-driven controls need natural sizing review |
| Periodic application | Modern and historical-resource modes; independent main-window, menu, dialog and control validation | Startup/resource modes and native hierarchy checks implemented; full visual matrix pending |
| Acceptance and publication | Full native/visual matrix, ABI cohort rebuild, documentation, both remotes, cleanup | Pending |

## Completion gates

- Modern light/dark, both densities, 1x/1.5x/2x; targeted 3x geometry checks.
- All applicable normal, hover, pressed, selected, focused, disabled, mixed and
  default-action states; keyboard, pointer capture, callbacks and accessibility.
- Large fonts, long/multilingual labels, RTL, narrow windows and live updates.
- SVG and non-SVG configurations; classic and application-override regressions.
- Actual-window evidence from the verified gallery and Periodic processes,
  with executable/library identities; no demo-only styling masking defects.
- Coherent native ABI rebuild when records change; qualified source commits and
  documentation to both publication remotes. Render documentation after the
  overall feature is finished, not after each foundation change.

## Planned cross-application visual review

Use the Periodic Table of Motif as a mandatory second reference application
throughout implementation, alongside the gallery. Its dense mixture of native
widgets, gadgets and UIL-created controls should expose differences that an
individually arranged gallery sample can hide. This is a validation requirement
for the plan, not a claim that the remaining review has been completed.

1. Capture a baseline from current verified builds of both applications using
   the same profile, density, font and scale. Record source revision, executable
   and library identity with each capture. Keep historical-resource Periodic
   captures separate from the modern-default acceptance set.
2. Compare typography, text baselines, padding, minimum sizes, borders, focus
   rings, corner treatment, arrows and disabled contrast across adjacent control
   families. Inspect the main window, menus, notebook pages and every reachable
   dialog, including scrolled content and expanded containers.
3. After each toolkit work package, repeat the affected comparisons in both
   applications. Exercise keyboard navigation, selection, default actions,
   resizing and live theme/density changes in the actual running windows.
4. Assign each discrepancy to the layer that owns it: toolkit rendering,
   measurement and default resources belong in the library; demonstration
   content, grouping and semantic role selection belong in the application.
   Do not use application-specific colors, fonts, dimensions or drawing to hide
   an inconsistent modern control.
5. Finish with the full completion matrix above. Keep a concise issue ledger
   identifying control, state, reproduction, owning layer, regression coverage
   and before/after evidence. Both applications must pass before accepting the
   visual overhaul.

Sequence this review with delivery: shared typography/metrics and roles first;
buttons, toggles and segmented groups next; scrollbars and editing/navigation
controls next; dialogs and glyphs next; then remove gallery styling and run the
full cross-application acceptance pass. Recheck Periodic at each stage rather
than postponing it until the final gallery cleanup.

## Historical implementation checkpoints

The entries below record development stages in order. Their pending-publication
notes describe those stages, not the final integration status linked above.

### Accepted foundation evidence

The construction/live parity fixture reproduces the previous build failure
(`control0.background`: startup 14870251, live 16185595). The unified engine
passes all three profiles across ten classes. Existing classic-profile,
live-transaction/reentrancy, gadget lifetime and allocation-failure rollback
fixtures also pass on the same rebuilt library.

Density selection now uses the same transaction as profile selection. Native
coverage includes both startup orders, light/dark/high-contrast, horizontal and
vertical RowColumns, cached gadget geometry and overrides, equal-valued
SetValues assignments, callback/value preservation, reentrancy, density rollback
and scaled spacing queries. A failing native case exposed RowColumn copying old
sibling padding into newly updated controls; cross-child normalization now runs
after all resources are committed. Secondary gadget resources are included in
appearance resolution, with ownership snapshots taken after Xt construction.

Construction now establishes normal Xt/application resources before applying
internal profile defaults. Nine native parity cases cover thirteen classes:
light/dark/high-contrast, each with default resources, broad application
wildcards, and an explicit private colormap on an 8-bit X server. The cases also
verify modern-created controls restored to classic against independent classic
controls, and resource-database edits made after selecting a profile. Gadgets
inherit an unstyled parent-color baseline and use the same cached Xt pixel
conversion as ordinary resources.

Role resources and live subtree updates now have native C/UIL coverage in the
ABI 9 working branch. Typography coverage includes direct font aliases and
inherited BulletinBoard label/button/text defaults; PushButton initialization
prehooks require provenance capture before ordinary object registration. This
package remains unpublished while composite inheritance, scale integration,
non-Xft behavior and native-consumer qualification are incomplete.

The ABI 9 working branch now uses a common 96-DPI scale for typography and
logical profile metrics. Native tests cover 1x/1.5x/2x/3x live/fresh parity,
widget/gadget indicators, explicit font/dimension preservation and failed-font
preparation rollback. Existing installed scale override, rollback, reentrancy
and scrolled-layout checks pass. Full consumer and feature qualification remains
required.

Next foundation gates include complete composite font ownership and measured
natural control geometry. The complete historical-widget, visual and interaction
matrices remain required; representative resource parity does not replace them.

Periodic now exposes toolkit defaults independently of its historical resource
file. Native startup coverage includes its real UIL controls, menus and dialogs,
seven modes/override cases, invalid options and missing UID data. This reference
also exposed and fixed non-menu RowColumn shadow rejection and an unbounded List
geometry loop. Actual-window inspection additionally exposed and fixed the
string-tag sentinel regression responsible for missing default-font labels.
See `periodic-modern-appearance.md`; modern typography and control
redesign remain required before accepting its appearance.

## Periodic visual review after restoring text

The verified native light/dark main windows expose these remaining design gates:

- Small fixed-font labels and a weak title hierarchy amid oversized empty cells:
  supply the agreed shared render tables and font-driven natural geometry.
- Tiny checkbox/diamond indicators and inconsistent arrow sizes: redesign the
  shared selection controls, including their selected/disabled/focus states.
- Heavy gray scroll tracks with small arrow targets and weak thumb distinction:
  use coherent track, thumb and arrow metrics and state colors.
- Strong nested frames around notebook pages, inconsistent tab treatment and
  dark borders disappearing into dark surfaces: unify surface/border roles.
- Dense legacy bitmap dialog pickers and inconsistent icon backgrounds: use the
  shared modern SVG/fallback family while retaining historical mode.
- Spin, combo and option controls expose different text/arrow alignment and
  vertical padding: measure common content and align affordances consistently.

These are findings from the running application, not completion claims. Main
windows have been inspected; full menu/dialog interaction and DPI/state captures
remain required after the control redesign.

## Typography-stage actual-window review

The ABI 9 development Periodic windows have been inspected in modern light and
dark using verified executable/library identities. Shared sans-serif body text,
18 pt title, bold section headings and secondary subtitle improve readability
without application font definitions. These captures are incremental evidence,
not acceptance of the full design.

Remaining visible defects include clipped ComboBox content, undersized legacy
selection glyphs, oversized arrow samples, heavy scrollbar tracks, inconsistent
notebook borders and bitmap dialog previews. Composite font inheritance and
natural geometry need qualification before removing gallery styling. The
startup/teardown checks also exposed TabBox font-handle ownership and live
redisplay graphics-context defects; those have focused native regressions.

## Composite typography and initial geometry

The native composite regression now checks ComboBox text/list children, Scale
labels and internal TabStack tab boxes through role, profile and scale changes,
including explicit child/parent fonts. A separate startup check reproduced a
23-pixel text-field allocation for a 50-pixel font. ComboBox now recomputes its
natural dimensions before realization and retains explicit size ownership.
The actual 1.5× Periodic window now shows the ComboBox's full `aleph` value.
The font-ownership, role, appearance-parity and density suites pass on this
Xft/SVG development build. Non-Xft qualification and the remaining control
redesign remain pending; this is not final visual acceptance.

## Notebook connection qualification

Standard PushButton and PushButtonGadget page flaps now have open page-facing
edges in every theme, with one selected-page outline even for zero-depth frames.
Modern page/footer/tab colors match, stack lines are restrained and density
provides inner padding. Focus survives expose and live theme changes. Native
pixel tests pass 128 configurations each on Xft/SVG and core-font/non-SVG ABI 9
builds. Actual Periodic classic, scaled light and dark windows were inspected.
This fixes the Notebook connection/footer defects; it does not qualify the
remaining control redesign, clipped scaled section labels, overflowed radio
samples, tab-strip overflow or legacy dialog previews.

The normal demo uses horizontal `XmBOTTOM_RIGHT` placement, giving top-down
right-side tabs. Reversed placements remain intentional regression coverage.

## Modern toggle rendering in development

A shared native renderer now supplies full-size rounded checkboxes, circular
radio indicators, check/cross and mixed glyphs, and disabled state colors for
ToggleButton and ToggleButtonGadget. Modern palettes own unselected surface
colors as well as selected accents; explicit resource overrides remain tracked.
Both densities have two-pixel focus and natural height floors of 34/28 logical
pixels through indicator/margin geometry. The classic Periodic screenshot is
byte-identical before/after this renderer change.

Native state, RTL, keyboard and geometry checks pass 128 combinations each
on Xft/SVG and core-font/non-SVG builds. Nine startup/live appearance parity
cases pass. Actual modern light/dark Periodic windows have been inspected.
Selected/mixed checkbox presentation and the broader pointer/state matrix still
need application-level visual qualification. Scaled RadioBox clipping remains a demo
layout issue; the corrected control metrics must not be reduced to conceal it.
Switches, segmented groups, ordinary buttons and the complete input/state matrix
remain outstanding. This package is not yet published.

## Content-sized Periodic overview

The working demo now uses measured column/row tracks within an automatic
viewport, preserving its periodic-table arrangement. The initial viewport is
at the top left, keyboard traversal reveals obscured examples, and resizing
cannot squeeze frame headings or radio options below their content bounds.
All eight startup/profile modes, 1.5x scaling and narrow/wide resize round trips
pass native checks. Actual light/dark/scaled windows were inspected. At larger
scales the full eight-column table deliberately requires scrolling in a 1440px
window; documentation captures must distinguish viewport views from whole-table
views. Classic's main-window capture remains unchanged.

The responsive layout exposed a library fit-threshold oscillation in automatic
ScrolledWindow. The isolated correction is published as `539b54fc` with native
before/after evidence for both axes, all four profiles and four scales. The demo
layout itself remains unpublished with the ongoing visual-overhaul package.

## Segmented control implementation checkpoint

The working renderer now supplies a continuous modern outline and rounded outer
corners across the ordinary ToggleButton children. Interior boundaries have one
divider; selection uses a tonal surface and focus is drawn inside the segment.
Child construction no longer marks internal shadow/focus defaults as application
resource overrides. Classic and high-contrast retain the ToggleButton renderer.

Native geometry and pixel checks now pass with Xft/SVG and core-font/non-SVG
builds. Verified gallery windows in modern light and dark show the connected
outline and tonal selection. Inspection exposed an Xft Label background clear
that erased the selected surface behind text. The internal label draw interface
now lets a caller with an already-painted surface suppress that clear; native
pixels and recaptured gallery windows confirm the correction. Focus-border
painting is separate from label painting to prevent recursive expose calls.

Native state coverage now includes 1x/1.5x/2x/3x, both densities and reading
directions, live light/dark transitions, explicit selection-color ownership,
and native press/cancel/release action paths. The expanded 40-case matrix also covers pixmap-and-text segments in both
Xft/SVG and core-font/non-SVG builds. It verifies the icon pixels and label
surface as well as selection, focus and pointer actions. The pixmap redraw path
now delegates to the composite after choosing the state pixmap, preserving the
connected surface and combined text/icon placement.
It exposed a disabled-state defect: selection remained in the model but its
fill disappeared. The renderer now retains the selected surface when disabled.

This is not full visual acceptance. Required follow-up includes actual scaled/
compact/disabled gallery inspection and broader application integration.
Natural sizing now follows item/label/font changes while preserving explicit
width and height independently; zero restores toolkit ownership. The 43-case
native matrix verifies initial sizing, label growth, live scale changes and
restoring dimensions independently. The gallery no longer supplies fixed
segment dimensions, and its verified normal/scaled windows have been inspected. TreeView's stale private GC colors were corrected and the gallery's dark
navigation is now readable in a verified capture. Its custom styling still
needs removal. The repeated-X-display
Xft failure has been handed to the bug-fix peer separately.

## Gallery toolkit-default checkpoint

The gallery shell now uses toolkit appearance roles for page titles, section
headings, secondary text and source code. Its hardcoded font/render-table
fallbacks, colors and control shadow overrides are removed. Example/Source/
Built-in Icons navigation uses the toolkit SegmentedControl and its selection
callback, with a native page-visibility check. The obsolete tests demanding
private gray tab borders are replaced by a shared-defaults contract check.

Eight native startup/navigation smoke cases pass: four appearance profiles at
1x and 1.5x. The full modern-light integration also passes with 94 available
components, 105 metadata entries, page navigation, keyboard traversal and focus
restoration. Live theme/DPI controls remain explicitly pending qualification. Verified normal and scaled gallery captures show toolkit typography
and connected view selection. The modern ScrolledWindow now defaults to zero outer shadow thickness, removing
the recessed preview frame through toolkit resources. Explicit application
frames survive live profile changes; classic/high-contrast framing is preserved.
Eight startup cases and their live transitions pass in both build configurations.
Verified light, dark and scaled gallery windows confirm the frameless preview.
Navigation now uses measured label widths and font-derived row heights.
The inner form fills the Sidebar content, and viewport changes keep the selected
row visible without duplicate selection callbacks. The verified 1.5x capture
shows complete category labels and the selected Segmented Control row. Native
TreeView metrics checks pass in Xft/SVG and core-font/non-SVG builds, including
explicit row heights, restoring automatic metrics and live scale changes.
Private gallery styling must not return to mask remaining toolkit defects.
Full rendered docs remain deferred to feature completion.


### Tree hierarchy ordering checkpoint

The gallery exposed a toolkit ordering defect: visible rows followed the flat
input array, separating descendants from their category when roots were listed
first. TreeView now normalizes accepted input into stable depth-first order
using an iterative traversal. Root/sibling input order is preserved, including
models with children before their parents. The native ordering regression fails
against the prior implementation and passes 16 cases in each font configuration;
mouse selection, Up/Down navigation and collapse/reopen share the same order.
The actual 1.5x gallery window confirms that child components sit immediately
under their category. Modern disclosure sizing is now qualified separately below; selected-row styling remains open.


### Modern disclosure checkpoint

Modern TreeView uses scale-aware chevrons with shared measurement/drawing/hit
geometry and mirrored RTL direction. The first disclosure click expands or
collapses without selecting the item. Native pixels, callback counts, unchanged
selection and tiny-row clipping pass 16 cases per font configuration at
1x/1.5x/2x/3x. Hierarchy ordering and font metrics regressions also pass, as do
existing TreeView source guards. The final library build is recorded and the
actual gallery light/dark/classic/scaled windows were captured against its hash;
the scaled window confirms readable chevrons centered beside category labels.
Selected-row surfaces, search fields and the Sidebar header still need work.


### Tree selection and focus checkpoint

Modern selection now uses the shared `selectColor` surface, with application
color ownership and selected text background preserved across live profiles.
Native focus-pixel testing exposed a missing core translation table: logical
TreeView focus did not run Primitive focus actions. The standard translations
now supply focus/Tab handling, while consumed tree keys suppress duplicate
inherited traversal or parent activation. Final color/focus, disclosure and
hierarchy regressions pass in Xft/SVG and core-font/non-SVG builds. Verified
light/dark/classic/scaled gallery windows show the selected-row surface.
The private record additions belong to the still-unpublished ABI 9 cohort.
Search fields and Sidebar header controls remain pending; this checkpoint does
not close the complete visual acceptance matrix or publication gate.


### Text frame checkpoint (geometry qualification incomplete)

TextField and Text share a modern frame renderer that paints a flat border
inside their allocated shadow band. Modern defaults use a one-logical-pixel
border and two-logical-pixel focus area; dark borders use the shared subdued
border color. Explicit border width/color resources remain authoritative.
Editing, insertion cursor and text layout continue through the existing widget
renderers. Classic/high-contrast retain their bevel drawing. Native qualification
covers both widgets, palette/density/scale variants, editing, disabled state,
focus pixels, explicit overrides and restoration of classic/high-contrast.


Text geometry issue found during frame qualification: explicit pixel width is
not retained through initial modern font application. The captured native
fixture grew XmText to 2654 pixels at 2x despite an explicit 560-pixel request;
its X window and Xt geometry agreed, so this was not a stale capture. The
32-case column-sized field frame matrix passes in both Xft/SVG and core-font/non-SVG builds, but does not qualify explicit
geometry. Preserve the reproducer and correct Text/TextField dimension ownership
before accepting the editing-control package. The gallery's constrained search
field confirms the flat frame visually; this does not close the geometry issue.


### Dimension ownership correction

The text-width defect described above is now corrected in the shared appearance
layer and TextField SetValues. Per-axis constraints survive initial fonts, live
profile/scale changes and equal-valued setters; zero releases a constraint.
The resource-file tests also reproduced stale Xt database lookup after appearance
replaced the Xrm database. Successful commits now merge staged values into the
existing database, preserving its identity and ownership.

Both font configurations pass 34 dimension cases, 32 field-frame cases,
composite-font and segmented-geometry regressions, plus appearance parity,
density and role suites. The gallery text examples now use column sizing and
measured label/help spacing; verified actual light/dark/classic/scaled windows
confirm that the previously clipped scaled Text example fits. The complete
editing-control and cross-application acceptance matrices remain open.

### Sidebar layout checkpoint

Sidebar now measures its title and square directional toggle, hides the title
when collapsed, and bounds children to tiny allocations. The native header
matrix passes 48 cases per font configuration, including RTL, accessibility,
focus restoration and destruction of the saved focus widget.

The gallery now uses a Form with the workspace attached to the sidebar, so
collapse reallocates space within the existing window. Its integration test
asserts that the workspace gains exactly the width released by the sidebar,
then checks restored width and focus. The full gallery integration passes
(94 available components, 105 metadata entries). A pointer-click capture of
the verified 1.5x process confirms the narrow rail and expanded workspace.
The build took 2.22 seconds against the comparable 2.11-second incremental
baseline; the capture verifies executable and loaded-library fingerprints.

Follow-up qualification reproduced and corrected missing descendant Ctrl+M
dispatch. The native matrix now covers late-created and nested descendants,
Caps Lock, normal text input and focus restoration. It also exposed vertical
window shrinkage during collapse: width negotiation now preserves the parent
height. All 48 cases pass in each font configuration, and real key input in the
verified scaled gallery collapses the sidebar from its search field. The shared
ArrowButton renderer still retains its legacy bevel and is a remaining visual task.

The actual gallery keyboard capture caught an additional issue absent from the
isolated fixture: width redistribution invalidated the toggle's old position
and moved focus into the workspace. Collapse now assigns toggle focus after
geometry negotiation. The gallery integration asserts that focus remains on
the collapsed toggle, in addition to restored focus after reopening; the
complete integration and both native header matrices pass with this correction.

### Shared arrow rendering checkpoint

Modern ArrowButton and ArrowButtonGadget now share a flat rounded frame and
14-logical-pixel chevron, including pressed and disabled painting. Frame colors
come from the normal resources, with visible light/dark defaults. Glyph padding
and line caps stay within the allocated face, and the existing focus region
remains separate. Classic/high-contrast retain their previous renderer.

Both font builds pass 96 native interaction/pixel cases and the bounds matrix
through 3x; existing arrow drawing/allocation safety guards pass. Initial actual
gallery captures confirmed the shared sidebar/component renderer and exposed a
too-dark border inherited from the old dark bevel palette. Class defaults were
corrected and both matrices rerun successfully. The old bounds matrix required
the documented Apply API for repeated profile changes on one display.

This checkpoint does not close arrow-control qualification: automatic modern
target sizing and removal of the gallery's fixed arrow dimensions remain, along
with expose continuity during a drag outside an armed control and broader
composite integration. The overall overhaul and publication gate remain open.

The drag-out expose gap is now reproduced and corrected: ArrowButton and
ArrowButtonGadget retain pointer-inside state separately from armed state.
Forced expose remains unpressed outside, re-entry presses again, and activation
still follows the existing callback path. Both font builds pass the expanded
96-case matrix, including classic/high-contrast repaint comparisons, and the
bounds matrix through 3x. Automatic target sizing remains the next arrow task.

### Arrow target sizing checkpoint

The toolkit now owns automatic arrow target sizes and reports them through
QueryGeometry: 36 logical pixels for comfortable modern density, 28 for compact,
or larger when explicit frame/focus widths require it. The existing appearance
ownership registry handles each axis, including resource-file dimensions,
equal-valued setters, parent allocation changes and zero to restore automatic
sizing. Geometry is prepared using the incoming profile/density, so live
transactions do not calculate against the old profile.

Both font configurations pass 48 geometry cases, the expanded 96-case renderer
matrix and the bounds matrix through 3x. The gallery no longer sets arrow width
or height; its source excerpts are regenerated, its integration suite passes,
and the verified scaled window shows the toolkit's square target. The complete
overhaul, composite integration and publication qualification remain open.

### Scrollbar presentation checkpoint

Modern ScrollBar now paints an eight-logical-pixel rail inside its full hit area,
with a rounded thumb and bounded 24-logical-pixel minimum length. Defaults retain
an end arrow on each side, rendered as bounded chevrons with the existing
actions and no outer bevel. Explicit arrowless configurations remain supported. Appearance now owns unconfigured troughColor, showArrows and
sliderVisual resources and restores classic/high-contrast baselines.

Initial actual-window captures exposed an oversized thumb in the gallery's tall
sample and an invisible embedded Scale thumb. The renderer now separates rail
thickness from allocation and provides a visible thumb for the shadowed slider
variant. Fresh light/dark Periodic Table and light/dark/classic/scaled gallery
captures verify the resulting presentation against executable/library hashes.

Both font configurations pass 24 native scrollbar cases (three scales, light/dark,
both orientations and directions), including drag callbacks, keyboard increments,
startup/live appearance, disabled value changes, resource ownership and minimum
thumb length. ScrolledWindow, modern-arrow and dimension regressions and appearance
parity/density/roles also pass. The ScrolledWindow fixture was corrected to use
Apply for repeated profile changes instead of the startup-only Set API.

Scale-specific framing/markers, buttons, spin/combo controls, tabs and dialog icon
integration still require work. The overall visual overhaul remains unfinished;
these development sources are not yet a published ABI9 release.

### Button frames and label geometry checkpoint

Modern PushButton/PushButtonGadget share rounded flat borders, pressed fills,
default-action emphasis and density padding. DrawnButton uses the same frame
without repainting its application-owned center. Menus and Notebook flaps keep
their distinct painting paths. Pointer-inside state now survives exposure during
an armed drag for all three classes; the private records belong to epoch 9.

The DrawnButton custom-content probe exposed a geometry defect rather than a
center-paint defect: appearance margin changes reduced an explicitly requested
160x80 allocation to 22x12. Label and LabelGadget SetValues now recognize an
explicit equal-valued dimension argument. Both font builds pass the expanded
94-case dimension matrix, including label/button classes, mixed constrained axes,
resource dimensions, live profile/scale changes and zero-to-natural sizing.

The button matrix covers 72 widget/gadget role/density/scale combinations plus
24 DrawnButton custom-content cases. Normal, pressed, disabled and default states,
drag-out expose, classic/high-contrast repaint continuity and explicit DrawnButton
frame widths are exercised. Broader scrollbar, arrow, segmented geometry, sidebar
and appearance parity/density/role regressions passed during this checkpoint.
Notebook flap pixel checks pass in both font builds; their repeated-profile setup
was corrected to use the live Apply API.

The gallery's ordinary and disabled button/label sizes now come from the toolkit;
disabled/selected variants use measured heights and appearance spacing. Initial
and revised actual-window captures exposed the remaining fixed disabled width,
which has been removed. The Periodic Table shows the shared frames on tree and
standalone buttons. Its stretched standalone PushButton demonstration, default
button geometry, embossed disabled labels in dark mode, menu treatment, spin/combo
controls, tabs and dialog icons still
need qualification or redesign. The full overhaul remains unfinished.

The gallery smoke wrapper's ten-second timeout interrupted a successful 13.7-second
native run covering 94 available components and 105 metadata entries. Its bounded
watchdog is now sixty seconds and retains captured diagnostics on timeout. The
live theme/DPI gallery acceptance gate remains explicitly pending.

### Disabled text checkpoint

Label-derived controls now use readable modern disabled-text colors without
embossed shadow passes. The dedicated resource preserves application overrides
and supports UIL. The native matrix covers labels, buttons, toggles, widget and
gadget forms, menu accelerators, mnemonics, classic transitions and fallback
updates: 76 cases per Xft/SVG and core-font/non-SVG configuration. The button
regression also passes in both configurations. Verified gallery processes were
captured in light, dark, classic and 1.5x modes; light and dark windows were
inspected. This checkpoint does not close the remaining control redesign or
full-gallery live-profile/DPI acceptance work.

### Default-action geometry checkpoint

Modern default emphasis no longer reserves the legacy outer bevel. The native
reproducer previously grew a 122x38 button to 132x48; the new 28-case geometry
matrix preserves width, height, focus width, margins and label rectangles. It
covers widget/gadget, natural/fixed, constructor defaults, dialog preparation,
three scales and repeated classic/modern transitions. Both font configurations
also pass the 72-case button rendering/interaction matrix, including the repaired
gadget default-off repaint path.

The gallery now includes a default-action example using toolkit resources and
natural geometry. Actual verified processes were captured in light, dark,
classic and scaled modes; the light and dark windows show the emphasis inside
the same control height as an ordinary button. Full-gallery acceptance and the
remaining composite-control redesigns are still open.

For this checkpoint, the 94-case dimension-ownership regression and Notebook
connected-flap checks also pass in both builds. The gallery integration test
passes its 94-component registry, 105-entry metadata, navigation and focus
checks. Its live theme/DPI gate remains pending, and existing RowColumn
shadow-resource warnings still need resolution. The 1.5x actual window was
also inspected for text fit and default-button alignment.

### Periodic natural sizing and build recovery

The refreshed Periodic baseline still stretched the PushButton to occupy most
of its cell. Its UIL and the ToggleButton example now attach only their leading
and top edges, allowing the toolkit to choose their natural sizes. The native
Periodic bounds regression now compares both allocations to queried preferred
sizes after repeated window resizing. The native startup/resize matrix passes in both recovered font configurations,
and verified light/dark Periodic windows show the natural control sizes.

During this checkpoint the active remote build/source workspace disappeared
between a successful actual-window capture and the next source transfer. The
Mac source worktree and copied screenshots remain intact. A new isolated remote
workspace was cleanly rebuilt from that source, with explicit configuration and
source/output fingerprints. Both configurations include matching Xm/Mrm/UIL,
installed headers and both demo applications. Qualification records are copied
back to the Mac. Prior per-checkpoint results describe the earlier cohort.

The recovered Xft/SVG and core-font configurations each pass thirteen native
control/appearance suites (default geometry, disabled labels, buttons, dimension
ownership, Notebook, toggles, arrows, scrollbars, segmented geometry, Sidebar,
parity, density and roles), plus the Periodic startup/resize matrix. The gallery
integration checks also pass. Reused-display toggle/arrow/Sidebar fixtures now
use live Apply APIs instead of startup-only setters. These results qualify the
recovered development checkpoint, not the unfinished visual overhaul.

### Flat shared borders and accurate gadget color queries

Modern shared frames now use one logical pixel and equal top/bottom colors.
Native checks cover four painted edges, titled frames, three scales, explicit
application widths/colors, and live classic/modern transitions. RowColumn
variants that own their shadow geometry no longer receive illegal profile width
assignments. Dark toggle widgets and gadgets now share their indicator border.

The parity check also exposed a separate public-query defect: LabelGadget's
inherited synthetic shadow/highlight exports returned parent colors although
its cached paint colors differed. LabelGadget now exports its own three cached
colors, including for PushButtonGadget and ToggleButtonGadget. This also corrects
the original-color snapshots used during theme transitions.

Actual light/dark Periodic windows were captured from the verified current
process/library. The shared frames are flat; notebook tabs remain at the top
right. SpinBox/ComboBox glyphs, the dialog bitmap examples and the rest of the
composite-control work remain open. These captures are a development checkpoint,
not final documentation screenshots or approval of the complete overhaul.

The broader gallery check traced the remaining RowColumn warnings to
ColorSelector/FontSelector forwarding their own profile resources recursively.
A toolkit resource-commit guard now leaves descendant updates to their individual
plans. The expanded native regression verifies explicit child colors survive
profile changes and ordinary application updates still propagate. This is an
ownership correction, not warning suppression.

The final Xft/SVG checkpoint passes the 12-case border/selector regression,
composite fonts, appearance parity/density/roles, Periodic startup/resize and
gallery integration with no RowColumn shadow warnings. Both configurations
built successfully, but the active remote workspace disappeared again before
final core-font qualification started. Mac sources, actual-window screenshots
and a concise result summary survive. Final core-font acceptance and publication
remain pending; the earlier two-configuration results precede the selector
ownership guard and must not be represented as testing that final change.

The missing core-font qualification was recovered in a dedicated remote task
workspace outside the repeatedly emptied temporary parent. The clean build and
all seven final border/selector checks pass: borders, composite fonts, appearance
parity/density/roles, Periodic and gallery integration. No RowColumn shadow
warnings remain. The gallery's live theme/DPI gate is still pending; its core
font run emitted a separate fixed-font conversion warning. Records were copied
to the Mac immediately. This completes the shared-border checkpoint's two
configuration checks; it does not qualify the subsequent dialog artwork changes.

## Rounded surface rasterization checkpoint

Modern rounded button faces, checkbox/radio outlines, arrow-button faces,
segmented surfaces, scrollbar capsules and ToggleSwitch tracks/knobs share
`_XmAppearanceFillRoundedRectangle`. It samples circular corners at pixel
centers and mirrors the resulting horizontal spans across both axes. Radius
clamping handles tiny controls without changing the requested painted bounds.
The outer border and inner fill use the same primitive.

Native X server experiments confirmed that assembling four filled circles
with `XFillArc` at diameter `2 * radius` produces asymmetric corners. Reducing
the diameter to `2 * radius - 1` makes those masks symmetric but shrinks the
painted bounds; the smallest circle becomes empty. Changing that parameter
alone therefore does not satisfy the surface geometry contract.

`run-modern-rounding.py --prefix PREFIX --build-dir BUILD --out OUTPUT`
checks 504 native pixel cases: mirrored pixels and occupied bounds across tiny,
odd and even rectangles, plus real PushButton, PushButtonGadget and DrawnButton
faces and checkbox/radio surfaces in both modern profiles at 1x, 1.5x and 2x.
The build directory supplies generated private headers; the test loads the
explicit installed library and records its hash.

All 504 cases pass in both Xft/SVG and core-font/non-SVG configurations, along
with the existing modern border, button, toggle, arrow, scrollbar and segmented
geometry regressions. The core-font Periodic startup/layout matrix also passes.

The X arc call-site audit distinguishes retained artwork and thin outlines from
replaced filled corners and thick strokes. The only remaining direct calls in
`lib/Xm` are in Notebook and DrTog (rechecked after the Avatar rendering fix):

| Call site | Audit finding |
| --- | --- |
| Avatar | Former thin outline passed 156 native size cases. Circular images and initials now share the mirrored-span outline and clip; no direct arc calls remain. |
| Chip | Former thin half-circle outlines passed 156 native size cases. The measured-layout overhaul subsequently moved its capsule and focus ring to the shared mirrored-span renderer; no direct arc calls remain. |
| ProgressRing | Thick X arc strokes were asymmetric in 76 of 156 probe sizes; now uses the shared annulus/sector renderer, qualified below. |
| Spinner | Animated partial strokes now use the shared annulus/sector renderer, with stroke bounds and clipping qualified below. |
| Notebook | Intentional shaded spiral-binding artwork, separate from rounded control surfaces. |
| DrTog | Classic shaded radio indicators; modern indicators use the shared renderer. |

Periodic captures from the verified executable and library were inspected in
light, dark, 1.5x scaled and classic modes. Modern dialog artwork now has no
default stepped IconGadget frame. The XmTree connector-to-node gap was still visible at this checkpoint; the
separate geometry and repaint-ownership correction is documented below. This checkpoint does not complete the visual overhaul.

## Tree connector geometry and repaint ownership

Tree nodes reserve an outer focus margin. The connector previously stopped at
that child window's edge, leaving the margin visibly empty before the button
face. Connections now anchor to actual node coordinates and the primitive face
inset, with explicit X window borders treated as visible edges. Direct and
ladder layouts use the same endpoint contract in both orientations.

The parent GC retains child clipping. A separate GC paints only the endpoint's
focus margin inside its child window; it cannot paint across the face or other
nodes. Active keyboard focus remains visible. Child expose and focus events
schedule one coalesced tree-owned repaint. Each repaint removes old endpoint
fragments with the primitive unhighlight background contract before drawing the
current graph, so moving or resizing a node cannot leave the previous slope in
its window. Destruction cancels the pending work.

The native `run-tree-connections.py` fixture covers classic, modern light and
modern dark at 1x, 1.5x and 2x, both orientations, direct and ladder styles,
child expose, resize, stale-fragment removal, focus unhighlight and destruction
with repaint pending, including open/close controls and collapse/reopen. All
288 cases pass in both Xft/SVG and core-font/non-SVG configurations. Fresh
Periodic windows were inspected in light, dark, scaled and classic modes;
node borders are connected. These results supersede the earlier connector-gap
status above, while the broader visual overhaul remains unfinished.


## ProgressRing and Spinner stroke rasterization

The filled-corner audit also found reflection asymmetry in thick full-circle
`XDrawArc` strokes. ProgressRing and Spinner now share a pixel-center annulus
and angular-sector renderer. Full rings mirror across both axes and diagonals;
partial sectors are subsets of that same ring, with clockwise determinate
progress beginning at twelve o'clock. Stroke thickness extends inward from the
painted diameter. Row-span and half-plane clipping avoid temporary images and
per-pixel trigonometry. The C math dependency is detected by configure.

Both controls fit the complete stroke inside the allocation and center the
indicator when no external label is present. Spinner stroke and maximum
painted diameter scale with DPI. Focus remains a separate primitive outline.
Percentage text is omitted if it cannot fit in the ring's interior.

The native raster fixture covers tiny/odd/even sizes, stroke widths, reflection,
quarter-sector union, quarter-turn rotation, all spinner phases and actual
widget clipping at 1x, 1.5x and 2x in light and dark. All 1,945 cases pass in
both Xft/SVG and core-font/non-SVG builds, including corrected horizontal
centering. Existing ProgressRing and Spinner behavior fixtures also pass.

Six actual gallery captures were inspected against recorded executable/library
identities. The ring strokes and centering are correct, but the ProgressRing
percentage still uses a small fixed font and does not follow the surrounding
modern typography or 1.5x scale. Both controls initialize a private font copy
without a normal render-table resource contract. Correct font ownership,
startup/live appearance and DPI propagation are the next gate; this finding
prevents treating the component's full visual design as complete.


## ProgressRing and Spinner typography qualification

The fixed-font finding above is resolved. Both controls expose owned
`XmNfontList`/`XmNrenderTable` aliases, preserve explicit overrides, and follow
inherited typography and live scale changes. Natural geometry includes text
metrics; explicit dimensions remain stable. Failed font copies retain the old
render table. Gallery examples no longer set fixed dimensions.

Both configurations pass 88 native font/geometry cases, existing behavior
fixtures and the 1,945-case raster matrix. Fresh actual gallery captures were
checked against source, executable and loaded-library hashes. Inspection of
light/dark and scaled windows confirms modern percentage typography and proper
centering. These results supersede the font issue above; the full overhaul is
not yet complete.


## Embedded composite arrows

SpinBox and the drop-down variants of ComboBox use the shared modern chevron
renderer. The explicit region comes from the control's own hit rectangle;
painting preserves its existing hit testing, child layout, and callbacks.
Decorative padding reduces in half-height stepper regions before shrinking the
glyph. Pressed arrows invert the face and glyph; insensitive arrows use the
border color. Every state repaint clears the complete region, including old
classic shadow pixels. Classic and high-contrast profiles retain their renderer.

`run-composite-arrows.py` covers actual widget pixels across light/dark, three
scales, all five spin layouts, both orientations, both drop-down types and RTL.
It also checks pointer activation, per-arrow sensitivity and disabled/pressed
repaint. The standalone arrow fixture guards the shared renderer refactoring.
This change does not complete composite control framing and sizing review.

The final Xft/SVG and core-font/non-SVG builds each pass 816 composite cases
and 96 standalone arrow cases. Actual gallery and Periodic windows were
captured with verified executable, library and source hashes. Light/dark and
scaled review confirms the chevrons; the classic Periodic screenshot is
byte-identical to the capture before the padding refinement. Composite frames,
Scale styling and OptionMenu affordances still need further visual work.


## Scale track and thumb checkpoint

Modern Scale has a thin value track and a circular outlined thumb, painted
through its existing ScrollBar child. Repainting the full slider area removes
old thumb/selected-track pixels. Minimum-to-value coloring follows all four
processing directions. The engine's hit rectangle, value conversion, drag and
keyboard callbacks remain in use. The round thumb uses the shared symmetric
annulus renderer. Modern natural geometry is DPI-aware and resolves the target
profile during appearance transactions; explicit dimensions remain unchanged.

The native matrix verifies horizontal/vertical and both directions at three
scales in light/dark, live/startup natural-size parity, values moving both ways,
pointer callbacks, keyboard updates, disabled repaint, explicit markers/shadows,
thermometer marker retention and classic restoration. Both configurations also
pass scrollbar, composite-font, appearance parity, density and role regressions.
Actual gallery and Periodic windows were inspected using recorded source,
executable and loaded-library identities. The classic Periodic capture remains
byte-identical to the previous checkpoint.

Remaining Scale qualification includes live DPI with explicit fonts, full
focus/pressed feedback, ticks and constrained tiny allocations. Composite frame
and OptionMenu improvements remain separate unfinished work. This checkpoint
does not complete the full visual overhaul.


## Scale interaction and callback qualification

The explicit-font DPI probe reproduced missing relayout from 1.5x to 2x.
Scale now owns a display-scale callback and separate automatic-axis flags.
Geometry negotiation preserves explicit sizes and same-valued dimension
setters; fixed outer allocations still relayout their children. Modern thumbs
fill while pressed and return to an outline on release/cancellation. Primitive
keyboard focus remains separate and survives the track repaint.

The expanded native fixture checks multiple child callbacks, callback-list
mutation, pressed expose, focus pixels, explicit-font live DPI, fixed dimensions
and allocations from 1 through 16 pixels. Both rendering configurations pass,
alongside ordinary scrollbar and composite-font regressions.

Actual gallery input uncovered a separate crash that isolated tests had missed:
ScrollBar copied Xt's compiled callback storage as public callback records.
Removing that invalid snapshot and letting Xt dispatch its own resource fixes
the accessibility observer's corrupted closure. Six verified normal/pressed/
released captures in light and dark now complete and show the application
feedback message. The earlier failed actual-window run was diagnosed with GDB;
it is not counted as acceptance evidence.

Tick layout, cancellation edge cases and disable-during-drag still require
further qualification. Composite frames and OptionMenu styling remain pending;
the full visual overhaul is not complete.


## Scale motion, cancellation and tick alignment

Disabling or making Scale read-only during a drag now releases its ScrollBar
interaction immediately, including the keyboard grab and repeat timer. Escape
restores the initial value even if the keyboard grab was unavailable. A later
button release cannot emit a second completion callback. A dedicated ownership
bit balances the drop-site update bracket independently of keyboard ownership;
it uses an existing private flag byte without changing the record layout.
ScrollBar stops after callbacks that destroy it and cannot restart a repeat
that a callback cancelled by disabling the control or changing editability.

The native interaction fixture covers immediate and timed callback teardown in
classic, high-contrast and both modern profiles, both orientations, and all
three disable/read-only/destroy paths. It verifies the keyboard is available
from a second X connection and that no delayed callback fires. Scale's fixture
also checks cancellation, late release and real tick endpoint positions at
1x, 1.5x and 2x. Scale retains its declared slider extent instead of inheriting
the viewport scrollbar's minimum thumb length, keeping travel and ticks aligned.

Actual gallery captures exposed a further drag-paint defect despite passing
release checks: the shared copying helper recalculated the pointer position
from the previous value, then MoveSlider erased part of the newly drawn track.
Painting now consumes the supplied geometry, expose resolves committed-value
geometry, and erasure precedes painting. The regression checks the thumb while
moving in both directions before release or expose can repair the pixels.

This checkpoint leaves composite framing, OptionMenu styling and the complete
application/state/layout matrix open; it does not complete the overhaul.

Both Xft/SVG and core-font/non-SVG builds pass 256 Scale, 24 scrollbar
rendering and 48 interaction-lifetime cases. Ten actual light/dark gallery
captures use the verified rebuilt library; moved thumbs remain visible and
cancelled/released images are byte-identical. Source and output hashes are
recorded with the test receipts. Incremental builds compile ScrollBar.c and
relink libXm; the unchanged gallery executable loads that rebuilt library.


## OptionMenu disclosure checkpoint

The default OptionMenu CascadeButtonGadget indicator now draws a downward
chevron through the shared modern arrow renderer. It follows existing
text-metric allocation and RTL placement; explicit cascade pixmaps still draw
verbatim. Pressed and disabled states use the same color contract as other
modern arrows. Classic and high-contrast keep their original marker.

`run-option-menu-appearance.py` checks actual gadget pixels at three scales,
in both modern profiles, both layout directions and four enabled/armed states,
including the chevron's separated upper arms and joined lower tip. It also
checks supplied pixmap size and pixels. All 60 cases pass in Xft/SVG and
core-font/non-SVG builds. Actual Periodic light/dark/scaled captures show the
new indicator; the classic capture is byte-identical to the prior verified
classic capture. This change addresses disclosure only: overall composite
framing and the full application interaction matrix remain open.


## Composite editor frame ownership and SpinBox sizing

Modern SpinBox and drop-down ComboBox editors now default to zero inner shadow
thickness; the composite owns the outer frame. The plain ComboBox retains a
separate editor above its always-visible list. Light composites use a continuous
white input surface; dark composites retain their dark background. These are
profile resource defaults, not paint suppression, so explicit child borders
remain available. ComboBox propagates parent shadows during SetValues; the
appearance transaction now retains explicit child shadow values in preflight
and restores them after the parent commits, including equal-valued setters.

Actual gallery inspection found that SpinBox initialized preferred dimensions
only for unspecified axes. Fixed width and height left its preferred size zero,
so frame painting could draw nothing. GetSpinSize now always computes bounded
preferred dimensions while preserving supplied output dimensions. The gallery
also removes its arbitrary SpinBox width/height, demonstrating natural sizing.

The native frame fixture covers startup/live light/dark profiles at three
scales, natural/fixed allocations, all three ComboBox types, classic restoration,
explicit/equal-valued child borders and actual SpinBox frame pixels. Composite
arrow and font regressions accompany it. Full interaction and focus review,
other composite layouts and the complete gallery matrix remain open.

Modern SpinBox frames enclose the entire allocation, so extra parent-granted
space cannot leave a field-colored strip outside the border. Classic retains
its preferred-size frame policy. The pixel fixture checks both near and far
corners for natural and explicitly sized controls. Both builds pass 408 frame
ownership cases, 816 composite-arrow cases and the composite-font fixture;
appearance parity, density and role suites also passed after the resource
changes. Actual gallery and Periodic windows were inspected; classic Periodic
pixels remain unchanged. Full control focus/interaction review remains open.

SpinBox now uses the same flat edge-rectangle renderer as text fields. Its
frame paints the complete allocation; transaction-time paints resolve the
target profile. The far-corner pixel fixture reserves a sufficiently large
shell so profile growth cannot place sampled pixels outside the visible area.
A separate allowShellResize probe also passes, with longer resize processing.


## SpinBox constrained-height spacing

A native probe reproduced a lower arrow at y=200 inside a 32-pixel-high
SpinBox. The vertical gap reduction used the allocated width when calculating
its deficit. LayoutSpinBox now uses the height. The regression checks both
arrow rectangles remain inside heights sufficient for two arrows and verifies
that widening the same control cannot change their vertical coordinates.
Coverage includes classic/high-contrast/light/dark, 1x/1.5x/2x, beginning/end
stacked layouts, RTL and four constrained heights. Allocations smaller than the
combined arrow extents, multi-child shrinking and the full focus/input matrix
remain separate qualification work; this fix does not claim those completed.

The corrected library passes all 192 constrained-height cases in both font
configurations, plus 816 composite-arrow and 408 frame cases. Fresh Periodic
windows were inspected with verified executable/library/source identities.
The isolated axis correction has been sent to the bug-fix workstream for
independent integration and publication; publication is not yet confirmed.


## Notebook fractional-scale and repeated-selection review

The flap regression now covers 384 configurations per build: 1x/1.5x/2x,
zero and nonzero frame depth, four profiles, major/minor tabs, both orientations
and all placements. Repeated page changes move keyboard focus to the selected
tab before checking inactive edges; a focused inactive tab legitimately paints
its focus indicator and must not be mistaken for a closed bevel. Both Xft/SVG
and core-font/non-SVG configurations pass the extended connection checks.

Actual gallery review at 1.5x found the Notebook example wider than the pane.
It now uses natural dimensions and default page padding, with short example
copy. The complete notebook, page scroller and tab labels fit the pane. This is
a demo-content/layout correction; it does not claim arbitrary long labels or
very narrow allocations are fully qualified. The full overhaul remains open.

Actual pointer selection captures at 1.5x cover Overview, Details, Activity and
return to Overview in light and dark. The selected flap remains connected and
its focus outline follows selection. The review also found missing interaction
feedback: the gallery subscribed to help instead of page changes. The example
now subscribes to `XmNpageChangedCallback` and displays the current page number.
The integration fixture changes the page through the public resource and checks
one callback plus the corresponding feedback text, rather than synthesizing a
help event for this component.

The updated gallery integration passes in both Xft/SVG and core-font/non-SVG
builds. Fresh 1.5x light Details and dark Activity windows display page 2 and 3
feedback respectively. Build records and capture receipts match the current
example sources and executable/library hashes.

## Modern TabStack and TabBox padding

Modern tabs now use 12-by-6 logical-pixel padding in comfortable density and
8-by-3 in compact density. These are toolkit defaults, shared by both modern
palettes; the gallery adds no custom tab styling. TabStack propagates the
resolved padding into its internal TabBox. The appearance transaction tracks
both tab-margin resources, scales owned defaults, restores classic margins and
preserves application-owned values, including equal-valued assignments.

The actual 1.5x gallery capture confirms readable labels with more space on
each side. This checkpoint covers padding; tab focus, selection rendering,
orientation, narrow layouts and overflow remain part of the broader review.

Native validation passes 480 padding/ownership cases in each font configuration,
including live density changes and live 1.25x scale changes. Appearance parity,
density and role suites pass in both configurations. Fresh actual gallery
windows were inspected in light at 1.5x and dark at 1x. Inactive tab outlines
are weaker in the dark capture; visual consistency remains an open review item.

## TabBox partial-damage repaint investigation

The apparently weak dark outlines were a repaint defect in both modern
palettes at 1x. A complete expose restored them. TabBox aliases its fill and
text GCs; drawing one label clears its clip. Subsequent fills escaped the
damage region while border GCs remained clipped, erasing inactive outlines.

Redisplay now copies Xt's damage region and expands it to complete intersected
tabs, matching the text/pixmap helpers' paint unit. It restores the fill clip
after each tab and clears the expanded background before drawing. A native
fixture compares every canvas pixel after narrow damaged strips with a full
redraw. All 288 cases pass with core fonts across four palettes, three scales,
four sides and three styles. Fresh actual gallery light/dark 1x captures show
complete inactive outlines without requesting a full-screen refresh.

The Xft matrix is **not passing**: modern rotated side-tab text triggers
`BadMatch` in `RenderCreatePicture` when its one-bit label pixmap reaches the
color Xft drawable path. This remains in the test and has been assigned to the
bug-fix workstream for a renderer/lifecycle correction. The full orientation
gate must pass before treating TabStack qualification as complete. Stacked
layouts, pixmap labels and full keyboard/pointer review also remain open.

The TabStack gallery now subscribes to `XmNtabSelectedCallback` and reports the
selected caption. Native gallery integration changes the real selected page
and checks callback count and feedback; both font configurations pass. Actual
pointer captures cover Details, Activity and return to Overview at 1.5x in
light/dark. The inspected selected pages show intact borders and visible focus.
These captures precede integration of the peer's Xft drawable/cache patches
and do not qualify rotated Xft labels.

The peer's renderer fixes through `115c2265d` have been integrated into the
working cohort: depth-one drawables select bitmap rendering, color drawables
avoid window-only queries, visual lookup uses the drawable's screen/depth, and
cache eviction is silent for absent entries. TabBox now evicts its cached Xft
draw before all three rotated-label bitmap destruction/replacement paths.
The previous color-pixmap heuristic was rejected during review; native
orientation qualification is being rerun on the integrated build.

On the integrated build, the full 288-case partial-damage matrix now passes
in **both** Xft/SVG and core-font/non-SVG configurations, including modern
rotated side tabs. This supersedes the failing Xft orientation result above.
The case matrix covers four palettes, three scales, four tab sides and three
tab styles. Broader stacked-layout, pixmap-label and keyboard review remains
open; this result does not claim those cases.

### Long-label clipping and stacked repaint coverage

The extended repaint fixture adds seven long labels, basic/stacked/static-stacked
layouts, text-only/bitmap/color-pixmap labels, and wide/narrow allocations. It
retains the four palettes, three scales, four sides and three styles, comparing
5,184 partial repaints against complete redraws. Run it with
`run-tab-repaint.py --prefix PREFIX --out OUTPUT --extended`; `--case` selects
one diagnostic case without changing the default matrix.

This found stray label pixels in a narrow, beveled, stacked layout at 2x.
Setting the core X graphics-context clip did not constrain Xft text when
`XmStringDraw` received a null clip rectangle. Both normal and disabled direct
label draws now receive the allocated label rectangle. Rotated labels retain
their bounded intermediate pixmap and clipped copy path. Enlarging damage
regions alone did not fix this defect and was removed.

The same matrix exposed a separate allocation error: the side-tab width clamp
assigned its result to the height. At 2x, a right-side stack allocated a
532-pixel tab area at x = -112 inside a 420-pixel window. The clamp now updates
the correct axis. Left-side page placement uses the horizontal margin, and
page geometry stays positive and bounded when margins and stacked decoration
exhaust the allocation. The fixture checks tab-area and active-page containment
and adds 432 layouts with unequal margins and sizes down to one pixel.

Both Xft/SVG and core-font/non-SVG configurations pass all 5,184 repaint
comparisons and 432 constrained layouts. Both full gallery integration suites
also pass, including tab selection feedback. Screenshot polling now retries
individual X-server observation timeouts within its overall deadline and
reports early process exits even when stderr is redirected to a file; behavioral
tests cover timeout recovery, process identity and deadline expiry.

Scrolled overflow and keyboard/selection coverage remain separate requirements;
partial-repaint equivalence does not establish that every label is reachable.

## Dialog spacing and preferred example sizes

The 1.5x gallery review found a SelectionBox showing only one of three sample
entries and Command hiding its third history entry. Both examples forced
fixed pixel heights. MessageBox, SelectionBox, Command and FileSelectionBox
examples now request preferred geometry; the three-item examples request
three visible rows. Command uses its public history resource names.

Modern toolkit defaults provide 16 logical pixels of comfortable outer padding
and 8 in compact density for these four classes. They omit the redundant
content-container shadow; individual fields and action buttons retain their
borders. These defaults use the shared appearance ownership/scale path, so
application-specified values and classic restoration remain explicit contracts.
The gallery integration checks the complete third sample row's bounds.

The 240 margin/border ownership assertions pass in each configuration, and
gallery integration passes at default scale plus 1.5x light/dark. The native
live-change run nevertheless logs invalid horizontal-scrollbar ranges; those
warnings remain an open List navigator defect assigned to the bug-fix
workstream. These assertions do not constitute warning-free qualification.

Fresh 1.5x SelectionBox captures show all three items. Its action row now lies
below the preview fold and is reachable by scrolling. The gallery's fixed
client dimensions at larger UI scales need a separate responsive-window
review; no claim is made that the complete dialog fits the current viewport.

## Gallery startup and preview sizing

The gallery now obtains its effective scale through the public
`XmGetWidgetScale` query and scales its default 1100 by 740 client accordingly.
It bounds the default to 95 percent of the EWMH desktop work area, falling back
to screen bounds when that property is absent or invalid. Explicit Xt geometry
and shell width/height resources remain authoritative; no MWM-specific frame
size is assumed.

The preview fills the available area above a fixed feedback footer instead of
capping itself at 356 pixels. Its canvas still follows the component's actual
allocation, so large examples remain scrollable in constrained windows.
The screenshot runner identifies the gallery by its published process ID,
avoiding internal toolkit windows which can share the application title and
inherit the same geometry resource.

`run-gallery-window-geometry.py` checks actual shell dimensions for 1x/1.5x/2x/3x,
explicit geometry and position, size resources, Xft.dpi, valid work areas and
invalid work-area fallback. The display-scale consumer fixture also checks
the new query, null arguments, resource fallback and the minimum default scale.
Its controls now have a real layout parent; attaching multiple managed controls
directly to one shell had incorrectly made the shell own their allocations.
Full visual-overhaul acceptance and publication remain pending.

Both Xft/SVG and core-font/non-SVG builds pass all ten native startup cases,
the effective-scale consumer test and gallery integration. Sixteen fresh
MessageBox, SelectionBox, Command and FileSelectionBox windows were captured
with verified executable/library identities. The inspected 1.5x SelectionBox
and FileSelectionBox show their complete action rows; the 2x dark Command
shows all history entries and its prompt.

Narrow-window geometry is honored, but this is not a responsive-layout
acceptance claim: at 900 by 650 and 2x, the sidebar and unwrapped header and
description still consume too much room. That layout remains an open gate.

## Compact gallery navigation

Below the space required for the expanded navigation and a 640-logical-pixel
workspace, the gallery switches to compact navigation. Opening navigation
replaces the workspace with a full-width navigation page. Activating a leaf
returns to the example. Keyboard selection alone keeps navigation open.
Widening the window restores the user's desktop expansion preference, including
an explicitly collapsed sidebar.

The compact header uses a shorter title and hides the redundant subtitle.
Component instructions become concise, while the class's construction remains
available in Source. The view selector uses Example, Source and Icons. These
are application layout/content decisions; widget colors, fonts and chrome still
come from the toolkit theme. The existing welcome and icon pages need their own
narrow-layout review; this change does not establish full-gallery completion.

Both native configurations pass 24 TreeView activation scenarios, the hierarchy
ordering regression, nine gallery responsive scenarios (three themes by three
scales), and full gallery integration. Current executable/library identities
were checked for nine compact component captures and ten navigation-interaction
captures. Actual light/dark windows show a full-width navigation page, successful
activation of the already-selected leaf, and Ctrl+M return to the example.
The gallery toolkit-default styling guard also passes. Documentation source is
updated; rendered documentation remains a final-feature gate.

## Welcome and icon page reflow

The welcome page measures lines with each label's actual render table and
preserves immutable source prose. Explicit paragraph boundaries remain intact;
word bytes are not split. A word wider than the available viewport remains
accessible on the scrollable canvas. Content geometry changes invalidate layout,
so labels cannot retain stale allocations after a resize. The canvas negotiates
child geometry before its final extent is set.

The icon catalogue now shows all nine built-in symbols in a natural-size grid
of up to three columns. It requests the effective display scale through
`XmGetScaledPixmap`. Builds without SVG retain the five legacy dialog bitmaps
and label the four vector-only assets as requiring SVG. Cells share an image-row
height so different bitmap dimensions cannot misalign their captions.

`tests/automated/run-gallery-pages.py --binary BINARY --prefix PREFIX --out OUT`
checks 54 native resize/content/icon scenarios per configuration: classic,
modern light and modern dark at 1x/1.5x/2x, with both pages resized wide, narrow
and wide again. Add `--expect-no-svg` for a non-SVG configuration. The checks
cover full text preservation, overlap, horizontal overflow, restored heights,
icon dimensions and fallback capability. Both configurations pass.

Actual narrow-window captures show readable reflow and access to the final
paragraph and icon row by dragging the scrollbar. Wheel input over the plain
canvas was still unavailable at this checkpoint; the toolkit correction is
covered by the automatic scrolling checkpoint below.

## SVG target-resolution rendering

The SVG loader now receives the requested scale before rendering. Built-in and
file-based artwork is rasterized directly into the final pixel dimensions;
the image cache does not rescale it a second time. The scale remains part of
the pixmap cache key, alongside colors, depth and screen. Installed images and
non-SVG sources retain their existing precedence and bitmap scaling behavior.
Both intrinsic and rendered edges are bounded to 8192 pixels.

Native tests first reproduced the former raster-enlargement defect, then passed
with the corrected library. They check new detail within 2x2 pixel blocks for
all nine icons and an external SVG, scales from 0.01x to 3x, cache reuse,
reference release and rejected oversized/malformed input. SVG-disabled builds
pass their corresponding file-rejection and legacy-dialog fallback tests.
Fresh process-verified gallery captures at 1.5x, 2x and 3x cover the nine icons,
including both modern profiles at 2x. This is a rendering checkpoint, not
completion of the full visual overhaul.


## TreeView semantic activation

TreeView's accessibility action now uses the same selected-row invocation as
Enter. A selected leaf activates; a selected group expands or collapses. The
bridge reports the action only for a sensitive tree with a selected row visible
in the expanded model. Expansion changes notify accessibility clients, and
callback-driven widget or bridge destruction is supported. Mouse selection,
keyboard selection and activation remain separate.

The native regression covers all four appearance profiles, three scales and
both directions. The gallery responsive integration also invokes the selected
navigation leaf through the accessibility API, checking that compact navigation
closes and the example becomes visible. This qualifies selected-row activation;
it does not establish complete per-node assistive-technology navigation.

Both Xft/SVG and core-font/non-SVG builds pass 24 native semantic scenarios,
24 existing pointer/keyboard scenarios, nine gallery responsive theme/scale
scenarios, and full gallery integration. The new native test fails against the
previous library because the selected row does not expose an action there.


## Automatic scrolling over plain content

[ScrolledWindow wheel handling](scrolled-window-wheel.md) now routes real wheel
input over ordinary descendants to the nearest automatic frame. Existing widget
and application wheel actions retain ownership, including press-only bindings.
Native tests cover custom bindings, nested frames, List/Text behavior, both
axes and reversed directions, range bounds, sensitivity and callback teardown.
The gallery uses this library behavior without custom scrolling code.

Both native configurations pass nine real-XTest theme/scale scenarios and full
gallery integration. Eight captures from four verified light/dark gallery
processes show the welcome and icon pages before and after wheel input. The
final welcome paragraph and final icon row are reachable without dragging the
scrollbar. This resolves the plain-canvas wheel finding from the reflow
checkpoint; broader overhaul acceptance remains pending.

## Scrolled tab-strip implementation and qualification

Modern profiles now request `XmTABS_SCROLLED` for TabBox and TabStack.
Explicit application modes retain ownership across profile changes. The
canvas occupies the space between (or beside) the arrow controls, clips tab
content to that viewport, and reveals programmatically selected or traversed
tabs. Scrolling arrows move the viewport without selecting another page.
`XmNtabArrowPlacement` is available on TabStack as well as TabBox.

`XmTabBoxXYToIndex` accepts TabBox-local coordinates.
`XmTabStackXYToWidget` accepts coordinates local to the supplied TabStack,
its TabBox or one of that TabBox's children, and translates them before hit
testing. Arrow controls return no page. A standalone TabBox has no owning
TabStack and therefore returns no page from the latter API. This public
function is compiled independently of the optional tear-off implementation.

The native `run-tab-scrolling.py` regression covers 96 combinations of four
profiles, three scales, four tab sides and two layout directions. It checks
real pointer activation of scrolling arrows, all arrow placements, selection
and focus visibility, disabled tabs, page removal, resize, tiny allocations,
public coordinate conversion and partial repaint identity. Separate checks
cover modern/default resource ownership. The regression now sends real Home/End, directional arrow and Space key
events after establishing native keyboard focus. This exposed and corrected
RTL reversal leaking into vertical traversal: vertical Down always advances
in visual order, while horizontal Left/Right follows the displayed RTL order.
Refreshed demo screenshots and broader interaction qualification remain
required before declaring this part of the overhaul fully qualified.

The real-keyboard matrix passed all 96 scenarios in both Xft and core-font
builds. Four newly launched, executable/library-verified gallery windows were
inspected at light 1x/1.5x/2x and dark 2x. The three-tab demo remains legible
and connected to its page. It does not exercise overflow visually; the native
long-label scenarios cover overflow behavior. The gallery still leaves an
excessive vertical gap between the example and its feedback, which remains
an open layout defect. These results are not full-overhaul acceptance.

## Compact example feedback layout

The example preview requests its content height plus the native scroll-frame
chrome, bounded by the available page height after reserving feedback space.
Feedback attaches below the preview instead of the bottom of the page. Large
examples retain their full canvas and automatic scrolling. Configure events
recompute the viewport after component or page geometry changes.

Before an old example is destroyed, its dependent Form attachments are moved
to the stable description widget. This prevents Form's deletion handling from
rewriting the feedback offset to preserve an obsolete absolute position.
The replacement preview then becomes the feedback anchor with normal spacing.
Native full-gallery and responsive validation must pass after this change.

Both Xft and core-font builds pass the full native gallery integration and
responsive-layout tests after the attachment fix (94 available components,
105 metadata entries). Fresh verified-process captures at light 1x/1.5x/2x
and dark 2x show feedback directly beneath the tab example. This closes the
large vertical feedback-gap finding above; other overhaul gates remain open.

## Notebook long-label qualification

A native constrained-allocation test reproduced an active flap moved wholly
outside the notebook when its label exceeded the tab scrolling area. The
layout now bounds that tab's major-axis allocation between the scroll arrows;
the label draws within the resulting tab allocation. Minor tabs additionally
reserve the page-connection inset used by their placement geometry.

The native flap matrix checks both selected pages with long labels after
resizing from 480 by 340 to 280 by 220 logical pixels, then enlarging again.
It verifies containment, actual page-connection pixels and recovery of each
tab's original expanded-window dimensions. The test restores the original
profile before these checks, so classic and high contrast are exercised as
well as modern light and dark; earlier long-label coverage followed a modern
profile transition only.

All 384 configurations pass in both Xft/SVG and core-font/non-SVG builds,
covering three scales, both orientations, all four page placements, major and
minor tabs, flat and default frames, and widget/gadget tabs. Each build checks
768 constrained selections and 768 restored-size selections. This establishes
containment, connection and size recovery, not the remaining Periodic Table
visual acceptance or complete long-label readability.

## Periodic notebook page review

The current Xft/SVG Periodic executable and UID were rebuilt together and
verified against source/output hashes before capture. Actual XTest clicks
selected Widgets (2), Layouts (3) and Toggles (11); repeated page-scroller
activation selected layout traversal pages 4 through 10. The resulting
modern-light windows were inspected, including their page number and status
text. Together with the initial Dialogs view, all eleven pages have now been
visited in the real application at 1440 by 1080. This is one profile and size,
not the full classic/dark/scaled/narrow acceptance matrix.

The major flaps remain connected while selecting each section. The Widgets
page still exposes heavy boxed hierarchy disclosure icons, unlike the modern
arrow controls elsewhere; that inconsistency remains to be fixed in the
owning toolkit widget. The overview extends below the initial viewport, so
full notebook footer/minor-tab review requires scrolling the overview. Direct
page-field typing did not change pages during this capture; investigate its
editing contract separately from the working arrow navigation.

Initial resource-selector and field-typing capture attempts did not change
the displayed page and are excluded from page-coverage evidence. Only the
verified pointer/arrow captures count toward this review.

## Container disclosure refresh

The Periodic Widgets-page review led to a native reproduction of incorrect
disclosure state after replacing `expandedStatePixmap` and
`collapsedStatePixmap`. Container's refresh loop read the proxy button's own
constraint state instead of the associated icon row. It now follows the row
association and skips the unassociated sizing gadget. Application-supplied
pixmaps remain authoritative. The regression replaces both pixmaps twice and
checks expanded and collapsed rows before and after state transitions across
four profiles, three scales and both text directions. The separate redesign
of the default modern disclosure glyphs remains open.

## Modern Container disclosure implementation

Container now resolves its internal disclosure gadget to the owning icon row
and uses the shared modern arrow renderer for theme-owned defaults. The glyph
points down when expanded and follows text direction when collapsed. The
default target has a 20-logical-pixel minimum size. The appearance engine tracks
expanded/collapsed pixmap ownership as opaque resources: it never assigns or
frees them. Explicit constructor, resource-database and Xt updates remain
authoritative, including equal-valued updates. Ownership transfer requests a
repaint so a former theme glyph cannot remain on screen.

Native coverage includes repeated custom replacement, live classic/modern
transitions, equal-valued ownership transfer, scaled size and actual corner/
repaint pixels. Qualification and current-application visual inspection are
required before accepting this new rendering path.

The modern disclosure regression now passes 24 scenarios in each font build,
including explicit equal-valued override repaint checks and a 20-logical-pixel
minimum target. Verified Periodic Widgets-page windows were inspected in
modern light and dark: default disclosures are flat chevrons on the container
surface, without the former boxed triangles or button-colored patches. These
views supersede the default-glyph finding from the earlier page review. Full
pointer/keyboard accessibility and remaining overhaul gates are separate.

Both full native gallery integration runs and responsive-layout checks pass
with the modern Container disclosure renderer in the current working cohort.

## Container disclosure input qualification

Native pointer input reproduced a disabled-row activation defect: the internal
button is a sibling of the icon row, so normal sensitivity inheritance did not
protect the row. The shared outline action now rejects insensitive rows before
changing state or notifying the application. Re-enabling the row restores
activation. Programmatic outline-state resource updates remain available.
Keyboard expansion/collapse now forwards the triggering event to the callback.

The disclosure test uses XTest mouse presses/releases and Ctrl+arrow key events,
not direct action calls. It verifies collapse/expand, disabled clicks, restored
activation, callback counts and event types, and direction-correct keyboard
operation. All 24 profile/scale/direction scenarios pass in both Xft/SVG and
core-font/non-SVG builds, alongside the existing pixmap ownership and repaint
checks. The pre-fix library fails on the first disabled click. These checks do
not establish completion of per-node accessibility or the full visual overhaul.

## Scrollbar end-arrow review correction

Fresh verified Periodic captures exposed a mismatch with the agreed visual
contract: modern resource defaults suppressed the end arrows. Both modern
profiles now default to `each_side`; explicit `none` overrides remain owned
by the application. The native scrollbar regression checks default startup/live
parity, both arrow actions and preservation of explicit arrowless settings.
Rendering and interaction suites pass in both font configurations.

Scaled light and dark Periodic windows were inspected after the change: arrows
are present in the overview, List and standalone scrollbar. The scaled capture
also exposes a remaining metrics issue: the full scrollbar allocation stays
narrow rather than following the agreed density/scale thickness. That needs a
library sizing correction; this default change does not close that gate.

## Scrollbar thickness and viewport ownership

Modern default thickness now follows scale and density: 18 logical pixels for
comfortable and 16 for compact. The appearance transaction adjusts only an
unconstrained short axis and preserves explicit dimension arguments/resource
settings using the same ownership rules as other automatic geometry.

A live embedded regression exposed two interactions. ScrolledWindow removes
navigator window borders after creation; restoring that pre-insertion border
made an otherwise valid thickness request fail. Appearance now leaves these
parent-owned borders alone. Automatic ScrolledWindow also now accepts a
thickness-only navigator request by recomputing the clip within its existing
outer allocation, rather than depending on its parent allowing enlargement.
Query-only requests do not mutate geometry.

Both native font configurations pass standalone/embedded density changes,
live scale changes to 1.25 and back, fixed outer allocation, query-only checks,
and explicit dimension preservation. Full gallery and responsive integration
pass in both builds. The verified scaled Periodic window shows wider overview
scrollbar targets and larger end chevrons. This supersedes the narrow default
thickness finding above. Orientation changes, very small viewport bounds and
application-defined scroller geometry still require targeted qualification.

## Scrollbar dimension reset and orientation updates

A native setter test reproduced a fallback to legacy fixed-pixel thickness when
an explicitly sized scrollbar was reset to zero. Scrollbars now use the same
immediate geometry-request path as arrow controls. Their preferred short axis
comes from the shared appearance thickness calculation; constrained axes retain
the current allocation and the parent keeps final geometry authority.

Both font configurations pass immediate size reset, orientation change and
explicit-axis preservation checks in the modern scrollbar matrix, along with
the existing startup/live scale, density and embedded viewport checks. The
arrow-sizing matrix also passes after extending the shared update path. This
closes the immediate short-axis update gap; tiny viewport and
application-defined scroller qualification remain open.

## Scrolled List/Text scale and parent-allocation contract

The extended consumer test reproduced stale 18-pixel navigator thickness in
application-defined scrollers after a live scale change. Thickness-only
geometry requests now recompute the appropriate automatic or variable layout
within the existing allocation. Scrolled List and multiline Text are covered
with both navigators present.

The same test found a second defect at a 2x-to-1.25x transition: when a fixed
200-pixel allocation equaled the previous DPI default, ScrolledWindow's scale
callback directly resized itself to 125 pixels, bypassing Form attachments.
It now negotiates its preferred size with Xt and records the accepted result.
Parent constraints remain authoritative.

Both font builds pass the extended modern scrollbar matrix, including 48
List/Text consumer cases per build, fixed Form allocation, positive content
bounds and separation from the navigator rectangles. ScrolledWindow appearance
regressions also pass. Both display-scale override fixtures and both full
gallery/responsive integration runs pass against the same built sources.
Tiny-viewport allocation and remaining overhaul gates are still separate.

## Tiny automatic viewport bounds

An exact-allocation native test reproduced a 1x1 ScrolledWindow whose clipping
window remained at (4,4) with size 2x2. The automatic layout now bounds the clip
origin and positive extent before computing navigator extents. Decorative
insets yield when they cannot fit. Empty-content layout uses the same rule,
avoiding zero-sized X windows and restoring normal layout when content returns.

The expanded fixture checks all four scrollbar placements, both text directions,
classic/high contrast/modern light/modern dark, and 1x/2x scaling. It shrinks
through 64, 32, 16, 8 and 1 pixel allocations, then grows to 100; each step
checks clip containment, removal/restoration of content, and agreement between
navigator extents and the actual viewport. This is clip-window qualification;
very small scrollbar arrow/track allocations remain a separate gate.

Both font builds pass all 64 configurations (384 size steps each), plus the
native wheel and modern scrollbar regressions. Full gallery and responsive
integration also pass against the same library builds.

## Modern scrollbar region partition

The native tiny-length test reproduced an out-of-bounds track even with arrows
disabled: a one-pixel allocation placed its one-pixel track at coordinate one.
Modern scrollbar layout now bounds both axes, partitions arrow and track spans
without overlap, and retains one positive track pixel. When two separate arrow
regions cannot fit, they have zero extent and do not paint or activate. Larger
allocations restore them normally. Each-side and near/far grouped arrows share
the same calculation, including reversed processing directions.

Thumb, trough and arrow hit tests use exclusive end coordinates to match their
painted rectangles; this prevents a neighboring region from consuming an arrow
click when there is no spare gap pixel. The expanded native matrix checks eight
lengths from 1 to 80 pixels, all four arrow modes, both orientations and reversed
processing in both modern profiles at three scales, including activation of
each available arrow. Classic rendering retains its geometry path; hit-test
bounds are shared and require the existing interaction regressions.

The wider Scale fixture exposed a geometry ownership regression from the new
scrollbar thickness defaults: Scale's internal scrollbar was independently
requesting a standalone thickness, altering an explicitly fixed Scale width.
Appearance now excludes Scale-owned sliders from that geometry path. Scale
continues to own its knob diameter and child allocation; standalone and
ScrolledWindow navigators retain the scaled scrollbar thickness defaults.

Scrollbar layout resolves the target appearance profile while a live appearance
transaction is committing. The tiny fixture also switches classic to modern
at a 1x1 allocation and verifies both track axes remain inside the control;
using only the still-published profile during that transaction would select
the legacy geometry path.

Both font configurations pass the 768 tiny scrollbar layouts, the Scale fixture
and scrollbar interaction regression, plus gallery and responsive integration.
The verified Periodic modern-light window was inspected after these changes.
This checkpoint does not qualify the remaining undersized ScrolledWindow
navigator allocations; those are tested separately below.

## Undersized automatic navigator allocation

The expanded native fixture reproduced a visible scrollbar extending beyond its
ScrolledWindow at small allocations. Constant layout now suppresses navigators
whose full cross-axis thickness, spacing and frame leave no positive viewport.
The remaining viewport and scroll extents are recomputed together. Each layout
starts from the configured policy, so increasing the allocation restores the
navigators; their preferred thickness is preserved.

The fixture checks visible navigator containment and separation from the clip
window across ten square, narrow and short allocations, including 1x100 and
100x1. Both navigators must return at 100x100. It also covers removal/restoration
of content, all four placements, both directions, four profiles and two scales.
This automatic-layout coverage does not establish tiny allocation behavior for
application-defined Scrolled List/Text layouts.

Both configurations pass all 64 automatic-layout configurations (640 allocation
steps each), plus native wheel and modern scrollbar regression suites. Build
records confirm ScrolledW.c was compiled in both incremental builds. These are
native geometry and interaction results; a new visual acceptance capture has
not yet been made for this allocation change.

## Application-defined tiny allocation checkpoint

Variable layout previously used unsigned dimensions and a two-pixel fallback
for negative remaining space. It now uses signed dimensions and the shared
positive viewport bounds, and moves managed navigators off-frame when their
thickness cannot coexist with content. Growth reevaluates their visibility.

The Scrolled List/Text fixture resizes the owning Form through 32x32, 16x16,
1x1, 1x100, 100x1 and 320x200 and verifies the parent-allocated size, positive
bounded content, contained non-overlapping visible bars and restoration.
An initial direct resize of the attached ScrolledWindow caused its Form to
restore the previous allocation; that fixture was corrected to resize the
owner and cannot serve as the before-fix reproduction for this checkpoint.

The diagnostic run also exposed a separate layout-reentrancy issue: List can
change managed scrollbar state while its work area is being configured,
leaving a vertical navigator shorter than the enlarged content. Bounds checks
do not qualify navigator-length consistency; this remains an open gate.

Both Xft/SVG and core-font/non-SVG builds pass the consumer bounds matrix
(48 consumers, six sizes each), the automatic viewport fixture and native
wheel regression. These results qualify the tested bounds and interactions,
not the outstanding reentrant navigator-length behavior or visual acceptance.

## List inner-width visibility correction

The short navigator observed in the preceding checkpoint was triggered by a
List calculation: SetHorizontalScrollbar cast negative available inner width
to Dimension, making overflowing content appear to fit. The resulting
unmanagement during child resize triggered another parent layout while the
outer layout still held its earlier dimensions.

The comparison now remains signed and treats negative inner space as zero.
The routine also checks the horizontal navigator's management state before and
after its update; it previously checked the vertical navigator by mistake.
The native fixture asserts that long List/Text content retains a managed
horizontal navigator across all six parent-driven allocations and that every
visible navigator matches the corresponding content dimension. Empty lists
still remove horizontal navigation and adding long content restores it.
This fixes the observed trigger; arbitrary callback-driven layout reentrancy
has not been established by these focused consumer checks.

Both final font configurations pass the native scrollbar suite, including
48 List/Text consumers with six allocation steps and the empty-list restoration
checks, plus full gallery navigation/focus and responsive integration. Build
logs confirm List.c was rebuilt and the native receipts identify the matching
library hashes. The gallery's broader live theme/DPI qualification remains open.

## Periodic DrawingArea frame simplification

The overview's DrawingArea was nested inside an explicit two-pixel inset frame,
inside its titled section frame, while its callback already draws the plot
boundary. Removing that extra Frame leaves the drawing directly under its
section frame. No custom modern rendering or color override is introduced.

Both font configurations pass the actual Mrm startup matrix, including light,
dark, scaled, compact, classic, historical resources, contrast and application
resource overrides. Logs show the UIL compiler regenerated periodic.uid and the
executable was relinked. Five fresh variants were captured from PID-, library-
and asset-verified application windows; modern light and dark were inspected.
The redundant bevel is gone. Overall cell density, control alignment and the
remaining complete visual acceptance gates are still open.

## Periodic overview alignment

SpinBox, ComboBox, horizontal ScrollBar, Label and Scale examples now align to
the top of their section work area. The previous percentage attachments placed
fields halfway down the row while buttons started directly under the heading.
Natural control height is retained; the Label no longer fills its entire cell.

Both native startup matrices pass, including the actual attachment and y-origin
checks in the Mrm hierarchy. UIL-only builds regenerated the changed layout
asset in 0.10/0.09 seconds; logs identify the UIL compilation and records retain
its source/output hashes. Five verified application variants were captured and
the modern light and 1.5x scaled windows inspected. The visible examples now
share a consistent top placement. The overall periodic-table layout remains
scrollable at larger scales; full spacing and visual acceptance remain open.

## Current documentation captures and menu-preview settling

The Periodic comparison tabs now contain the five verified alignment captures.
All 105 catalogue screenshot references resolve, and regenerated Demo/Source
blocks cover 37 unique documentation pages with 131 valid source-line includes.
Rendered HTML has not been rebuilt; that remains the final primary-checkout gate.

Refreshing every gallery capture exposed a menu-preview geometry loop: the
preview host was sized from its children's bounding box, which excludes the
RowColumn's own frame and spacing. The CascadeButton entry prevented the
idle capture-ready work procedure from running. Menu hosts now use their
queried preferred size; ordinary canvas hosts retain child-bounds sizing.

Two preceding batches stopped at the first CascadeButton. With the correction,
all 105 entries captured successfully from the verified current library and
executable. Their PNG hashes were checked before replacing documentation assets
and regenerating the publishable manifest. The obsolete raw receipt, which
referred to a different old asset set, was preserved in validation storage and
removed from the documentation directory. Actual CascadeButton, Notebook and
ToggleButton gallery windows were inspected. These captures refresh evidence;
they do not establish completion of the whole visual redesign.

Both Xft/SVG and core-font/non-SVG builds also pass full gallery navigation,
focus restoration and responsive integration after the preview geometry fix.
The source contract and screenshot-runner checks pass locally, and all 105
published component PNGs match their regenerated manifest hashes.

## Gallery row budgets

The refreshed ComboBox capture exposed excessive unused space beneath three
items. List and ComboBox examples now request three visible rows; List's fixed
110-pixel height is removed. This is dataset sizing, with row height determined
by the toolkit's fonts and spacing, not custom modern styling.

Both font builds pass the expanded integration check requiring three visible
rows and complete third-row bounds, plus navigation/focus and responsive checks.
Eight verified actual-window captures cover both controls, light/dark and
1x/1.5x. Light List and ComboBox and scaled dark ComboBox were inspected: the
three rows fit and the unnecessary blank list capacity is gone. Documentation
source excerpts are regenerated. The published screenshot set retains its
previous manifest-bound capture until the next complete gallery refresh.

## Symmetric TreeView disclosures

The modern disclosure used unequal integer arm spans inside even-sized slots.
Equalizing its endpoints still failed a native reflection check because the
rounded thick X line stroke produced different pixels on opposing arms.
The shared chevron primitive now samples distance to a 45-degree arm at pixel
centers, mirroring the coordinate before sampling. It batches horizontal fill
spans and leaves GC line attributes untouched. TreeView keeps its established
row-aligned slot, hit region, RTL direction and classic plus/minus renderer.

Both builds pass the expanded native disclosure matrix: 16 profile/direction/
scale combinations each, testing closed/open/closed pixel reflection, activation
and tiny-row clipping. Measurement and pointer/keyboard activation suites pass
as well. Initial verified gallery captures showed the balanced glyphs in normal
light and scaled dark, but also exposed a demo fixed-height defect hiding the
fourth tree row at 1.5x. That height override is removed; visible-row count now
provides the font-dependent preferred height, with an integration containment
assertion for all four sample nodes.

Removing the demo override also exposed toolkit height ownership: Primitive's
initialized fallback size was mistaken for a requested TreeView size, and modern
font application changed row height without recomputing overall height. TreeView
now checks original requested dimensions during construction and tracks automatic
height in its epoch-9 private record. Font/row-count updates compute natural
height through SetValues geometry negotiation; explicit nonzero heights remain
owned by the application, and zero restores automatic height. Native metrics
checks now compare actual height to preferred row-derived height and exercise
explicit-height preservation/reset. ABI documentation includes the new field.

Both final native configurations pass disclosure, metrics and activation suites
and full gallery/responsive integration. Four new verified TreeView captures
cover light/dark at 1x/1.5x; the scaled dark application window was inspected and
now shows all four sample rows. Evidence is retained in the TreeView validation
checkpoint. The wider visual-overhaul and ABI publication gates remain open.

## Shared modern arrow symmetry

The native ArrowButton pixel fixture reproduced reflection asymmetry in the
shared modern arrow renderer, beyond the previously corrected TreeView glyph.
The renderer now calls the same pixel-center chevron primitive. This also
covers ArrowButtonGadget, Container disclosures and composite arrow regions.
Existing face, palette, clipping, pressed-state and hit geometry are retained.
The test now checks glyph reflection within its actual ink bounds in every
existing normal/pressed/disabled/repaint/restore state and direction.

Both Xft/SVG and core-font/non-SVG builds pass all six native suites: modern
arrows (96 profile/density/scale/widget/direction cases per build, with multiple
interaction states), composite arrows, Container disclosures, option menus,
modern scrollbars and TreeView disclosures. Five Periodic variants were captured
from the verified current executable/library/UID; normal light and 1.5x light
windows were inspected. Evidence is retained in `shared-chevron-validation` and
`periodic-shared-chevron-current` in the development workspace. These results
qualify this renderer checkpoint, not the complete ABI-9 overhaul.

## Live gallery transaction qualification

`ckmotif-gallery --integration-live` walks every constructible available demo
and changes profile, density and scale on each realized example. It verifies
public settings, component identity, selection, callback count, active page and
preview containment. Live controls remain gated until this matrix and the
remaining interaction/visual checks pass.

The first run hung in ColorSelector in both native configurations. ButtonBox
recomputed proportional allocation from each accepted compromise, repeatedly
shrinking the child until a zero-size request restarted the cycle. Its pending
geometry offer now retains the original preferred dimensions. Accepting the
offer uses that same allocation; other layout or geometry changes invalidate
the offer. Query-only acceptance does not consume it. The private constraint
record change belongs to the pending epoch-9 cohort.

After that correction the matrix exposed a double free in FontSelector during
live scaling. RowColumn nested preferred-size/geometry requests replaced and
freed the active outer layout array. Each allocation-owning invocation now
saves and restores its caller's array. A native DrawingArea resize callback
queries its parent during layout to exercise this ownership contract.

The notebook page-number field is intentionally non-editable in
`SetPageScroller`; earlier failed typing attempts therefore do not demonstrate
a broken commit action. Arrow navigation remains its supported input.

The focused regressions pass in both Xft/SVG and core-font/non-SVG builds:
123 accepted ButtonBox compromises and 255 nested RowColumn queries each.
The live test also exposed its own invalid assumption that popup examples
have a preview canvas; canvas checks now apply only to embedded examples.
The live catalogue run remains a separate acceptance gate.

Both configurations subsequently completed all 376 live transitions across
94 available component demos. The real gallery instances and public state
remain intact through the tested sequence. This closes the newly added live
catalogue state/containment matrix; toolbar interaction, focus behavior during
those changes and full visual acceptance remain separate unfinished gates.
Evidence is retained in `live-geometry-validation`.

Existing full-gallery interaction and responsive-layout regressions also pass
in both configurations after these changes. The independent RowColumn fix and
native evidence have been sent to the bug-fix peer for separate qualification;
no publication is claimed here.

## Live toolbar controls and external focus

The gallery now exposes Theme and Scale controls using only public toolkit
transactions. Theme cycles classic/light/dark; Scale cycles 100/150/200/300%
and automatic sizing. Each callback reads current public state, preserving
correct behavior after command-line overrides or external setting changes.
There is no local counter to advance if a transaction fails.

Native activation checks exposed Sidebar stealing focus from the Scale button
when 150% sizing triggered the gallery's compact layout. Sidebar now relocates
focus on collapse only when focus was inside it. Expanding clears its saved
focus reference, but restores that widget only if the user has not moved focus
elsewhere. The header fixture covers external focus during collapse/expansion
and moving away while collapsed, as well as its existing keyboard, accessibility,
nested-sidebar, tiny-allocation and destroyed-focus-widget checks.

Both build configurations pass the expanded Sidebar fixture and the full
standard gallery runner, which now includes activation of every Theme/Scale
step with focus and state checks. Verified-process light, dark and enlarged
light gallery captures show the visible controls fitting the header. This
accepts the toolbar activation/focus checkpoint; the wider visual and interaction
matrix and publication remain unfinished. Evidence: `gallery-controls-validation`
and `gallery-live-controls-current` in the development workspace.

## Rounded button focus outlines

The visible Theme control exposed square focus geometry around an otherwise
rounded modern button. A native pixel assertion reproduced the square outer
corner. PushButton, PushButtonGadget and DrawnButton now share a modern focus
renderer using the same symmetric corner primitive as their faces. It clips
painting to the reserved highlight margin, so focus and unhighlight do not
replace label pixels or application-drawn content. Explicit highlight colors
and thicknesses remain inputs. Classic/high-contrast and notebook flap paths
retain their own focus rendering.

The expanded button fixture checks mirror symmetry and corner/background
coverage, full repaint identity while focused and clearing of the focus band.

Both Xft/SVG and core-font/non-SVG native button suites and default-button
geometry suites pass. Verified gallery captures in light, dark and enlarged
light show the rounded focus outline on the Theme control; dark and enlarged
light windows were inspected. This is a button-focus checkpoint, not completion
of the remaining cross-control visual/state matrix.

The standard full-gallery interaction, responsive-layout and toolbar activation/
focus checks also pass in both builds on this renderer. Build identities,
native receipts, gallery logs and actual-window captures are retained in the
button-focus validation checkpoint; documentation HTML remains unbuilt pending
completion of the overall feature.

## Core-font code typography

The core-font build requested a normal-width `fixed` face. The reference server
only advertises a semicondensed fixed family, and its synthesized 15/22/29-pixel
names cannot actually be opened. Merely relaxing the width match still failed.
The native code-role regression reproduced conversion warnings and unequal
advances for `iiii` and `WWWW`, proving a proportional fallback in code text.

Core-font appearance now prefers scalable Courier for the code role, probes
font candidates before constructing a rendition, and retains loadable fixed
family/alias fallbacks for limited servers. Missing candidates therefore do not
produce Xt converter warnings as part of normal fallback selection. Xft keeps
its monospace family selection. Explicit application font ownership is unchanged.
The native test checks four scales, both densities and both modern profiles,
live/fresh metrics, monospace advances, classic restoration and zero warnings.

Both builds pass the code-font matrix with their normal font server and a
server restricted to built-in fonts. On the restricted server, a readable
monospaced fallback is required, but scalable font growth cannot be required
when no scalable core font exists. Both also pass composite font ownership and
ordinary gallery regressions without FontStruct conversion warnings.

Inspecting the core-font Source tab at 1.5x confirmed monospaced enlarged code,
but exposed missing vertical scrolling: the gallery had left its read-only
Text widget in default single-line edit mode. The source viewer now explicitly
selects multiline mode through its normal public resource. Gallery navigation
checks verify the vertical scrollbar and that the last source position can be
scrolled into view. This is demo content semantics, not custom modern styling.

The final gallery interaction, responsive-layout and toolbar checks pass in
both configurations, including source scrollbar and final-line reachability
assertions, with zero font-conversion warnings. A verified core-font Source
window at 1.5x was inspected with the vertical scrollbar visible. Native and
build evidence is retained in `code-font-validation`; final actual-window
evidence is in `gallery-code-source-final`. An initial capture that failed to
select Source is excluded from this qualification. The overall overhaul and
publication gates remain open.


## Focused component source excerpts

The gallery and documentation now share `gallery_source.py` to select annotated
ranges from the actual demo implementation. Construction, additional control
states and field label/help layout are included where relevant. The Source tab
explains the host arguments and shared factory ownership rather than presenting
an unrelated callback dispatcher or the entire component factory.

DragOverShell shows the actual drag-source factory. CombinationBox2 and Ext18List
resolve to the DropDown and MultiList implementations used by their class aliases.
Abstract/service entries describe their role and point to public declarations.
Excerpts remain excerpts, not separately maintained standalone programs.

`check-gallery-excerpts.py` checks every manifest entry against exact source
ranges and documentation includes, relevant state/label examples, aliases,
special factories and rejected malformed annotations. Both generators consume
the same selector. Rendered documentation is refreshed at feature completion.

Both Xft/SVG and core-font/non-SVG builds pass excerpt/source/provenance checks
for all 105 inventory entries and native gallery navigation, responsive layout
and appearance-control checks. The native Source check compares the displayed
text with the selected entry's generated excerpt, replacing its former search
for an internal fragment ID. A verified core-font gallery window at 150% scale
was captured and inspected with Source selected: the button construction appears
first and both scrollbars remain available. Catalogue and component-page source
includes were regenerated; rendered HTML remains pending feature completion.


## Notebook direction matrix and complete reference viewports

The native notebook fixture now repeats its existing layout, connection-pixel,
focus, profile-change, constrained long-label and preferred-size-recovery checks
with right-to-left layout as well as left-to-right. Both Xft/SVG and core-font/
non-SVG configurations pass all 768 cases. This adds direction coverage; it does
not establish ellipsis/readability for clipped long text.

Fresh actual Periodic windows were inspected in classic, modern light, dark,
compact and 1.5x variants. The former fixed 1440-by-1080 capture cropped the
scaled table and placed the notebook footer below the viewport. Reference
captures now use a 1440-by-1200 logical viewport (2160-by-1800 at 1.5x), recorded
alongside native image dimensions and executable/library/UID hashes. All five
images show the complete overview and footer and replace the prior development
snapshots in the documentation tabs. These reference-sized images do not prove
constrained-window behavior or all notebook pages; those remain separate gates.


## Rounded arrow-button focus

The current Periodic review exposed a square keyboard-focus frame around the
modern chevron button. A native pixel test reproduced the painted outer corner.
ArrowButton and ArrowButtonGadget now use the existing shared rounded-focus
renderer for modern profiles, with their normal focus flags and classic/high-
contrast rendering paths retained. The renderer clips painting to the reserved
highlight band, preserving the arrow and pressed face.

The arrow fixture now checks empty outer corners, accent edge pixels, horizontal
and vertical reflection symmetry, full redisplay identity and focus-removal
cleanup, in addition to existing activation, pressed, disabled and legacy checks.


Actual-window review then found a second issue in the shared helper: a radius
larger than three highlight-band widths could leave four symmetric but detached
sides after clipping. The helper now bounds its radius to that reserved band.
The ArrowButton and ordinary-button fixtures additionally require all highlight
pixels to form one eight-connected outline. This assertion reproduces the
intermediate defect; it is distinct from the existing symmetry assertion.


Both native font configurations pass all 96 arrow cases and 72 ordinary-button
cases with connectivity assertions, plus gallery navigation, responsive layout
and appearance-control checks. Fresh verified Periodic windows were inspected
in modern dark and 1.5x light: focus corners are connected and symmetric. This
supersedes the intermediate disconnected-focus capture; it does not complete
the broader visual overhaul or its publication gates.


## Link sizing without a gallery repair

Removing the Link-specific construction repair exposed an allocation of 116 by
21 pixels for text measuring 146 by 20 in the modern Xft configuration. Link now
shares one measured sizing function between construction, live changes and
geometry queries. Width and height independently retain automatic ownership
until an application assigns a nonzero value; zero restores automatic sizing.
Explicit sizes remain authoritative in geometry queries as well as SetValues,
so a RowColumn relayout cannot replace them with the label's natural dimensions.

Padding reserves both visited-state underline rules and accounts for rounded
fractional-DPI gaps. Underlines follow measured text rather than fixed offsets
from the widget bottom. Drawing refreshes foreground/background colors and uses
the disabled border color when insensitive. Label copies preserve compound-string
structure. The private ownership flags belong to the pending ABI-9 cohort.

A classic-restoration regression also exposed geometry measured using the old
profile during live commits. Internal profile/density geometry queries now use
the target settings through both resource commit and subsequent cross-child
layout; public getters still expose committed settings until success. The Link
fixture covers DrawingArea and RowColumn parents, both modern profiles and
densities, four scales, visited/disabled pixels, multiline labels, equal-valued
explicit assignments, automatic-size reset, live updates and classic restoration.

The gallery's remaining size-repair branch still covers Prompt, IconLabel,
IconBox, Column, Disclosure and FormSection. Those controls require their own
measurement/ownership review before the branch can be removed completely.


## TreeView preferred size and allocated viewport

Review of the gallery's live appearance failure exposed a geometry ownership
error: navigation copied allocated tree height into `treeViewVisibleRows`, and
TreeView used that preferred-row resource to request another height. The gallery no longer
performs this synchronization. TreeView derives its drawn row count, wheel
bounds and selection visibility from its accepted allocation, while preserving
`treeViewVisibleRows` as a preferred-size input. Its resize handler adjusts the
scroll position without requesting geometry. Unpainted trailing space is not
an interactive row.

The native metrics regression now includes parent growth and shrinkage with a
fixed preferred row count. The first Xft run passes metrics, disclosure, activation, Link geometry,
live appearance/rollback, parity, density and regular gallery integration.
Removing row synchronization alone did not stop the event loop. The gallery now
also avoids reapplying unchanged sidebar constraints and label strings from its
ConfigureNotify handler. Two debugger runs then completed all 376 transitions,
one with normal address randomization and the integration working directory,
but a non-debugger run crashed after the FontSelector entry. That intermittent
failure is still under investigation. Subsequent ordinary runs pass all 376
transitions in both Xft/SVG and core-font/non-SVG configurations, together with
the targeted tests above. These passes qualify the viewport behavior but do not
explain the intermittent failure. A focused FontSelector lifecycle probe also
reports an invalid X ConfigureWindow request; that issue is being traced.

Fresh actual-window captures in light, dark and 1.5x light were taken for Link
and TreeView from the verified current executable/library. Inspection of the
scaled TreeView and dark Link confirms their labels fit the allocated controls.
Capture receipts retain process, executable, library and image identities. This
visual evidence does not resolve the intermittent integration crash.


## DropDown live-transaction geometry failure

A focused FontSelector lifecycle probe reproduced a native segmentation fault
and an invalid ConfigureWindow request that disappeared in debugger runs. A
synchronous native core located the invalid request in appearance commit to the
label inside FontSelector's DropDown: its height had become 60684. DropDown's
geometry manager read an uninitialized result when the parent refused its
requested height. Preferred-size calculation also read fields omitted from
child query masks.

The manager now restores speculative geometry and returns a defined refusal
when height cannot be provided. Preferred-size queries merge only supplied
fields with allocated dimensions and border width. The native regression
passes in both builds: 24 refused query/real requests, poisoned partial query
replies, and 48 FontSelector theme/density/scale transitions with allocator
memory poisoning. Full requalification now passes against this correction in both builds:
376 live gallery transitions each, regular navigation/responsive/appearance
controls, live transaction/rollback, parity and density. Fingerprinted build and
validation records are retained with the checkpoint. The isolated fix
has been sent to the bug-fix peer for independent review and qualification.


## Prompt and IconLabel measured sizing

Prompt and IconLabel now retain independent automatic width/height ownership,
measure compound text through their render tables, and preserve its formatting.
IconLabel no longer assumes a 16-pixel text height; its icon dimensions and
scaled padding contribute to the preferred size. RTL mirrors the icon/text
order. Its modern default gap follows scale and density while explicit spacing
remains application-owned. Explicit dimensions remain fixed until reset to zero.
The gallery no longer repairs either class after construction.

The new native label fixture failed against the preceding library on compound
string preservation. The Xft build now passes 64 configurations covering both
classes, DrawingArea/RowColumn parents, light/dark, LTR/RTL, four scales, multiline
labels, live changes, explicit dimensions and overflow. Both builds pass the 64 native cases, existing IconLabel/Divider fixture,
live appearance/rollback, parity/density, regular gallery checks and 376 live
transitions each. Verified gallery captures were inspected in scaled light for
IconLabel and dark for Prompt; their text is contained. The IconLabel sample
currently lacks an icon, so its demo-content review remains open.


## Gallery container defaults

Native containment checks show IconBox, Column, Disclosure and FormSection can
already size their contents without the gallery's fixed allocations and repair
pass. The initially proposed toolkit initialization edits were withdrawn:
allocation may legitimately exceed preferred size, and the controls pass the
relevant containment contract. The gallery now omits those example/child sizes
and removes its entire post-construction preferred-size repair block.

The added native container fixture passes 96 Xft configurations on the existing
toolkit: four classes, DrawingArea/RowColumn parents, classic/light/dark and four
scales, including live changes and disclosure collapse/reopen. Core-font and
actual-gallery qualification of the simplified examples are in progress.


### Visual review after removing container size repairs

Twelve fresh gallery captures were taken from the fingerprint-verified build
(light, dark and 1.5x light for all four containers). Inspection of scaled
IconBox/Column and dark Disclosure/FormSection confirms that the content fits,
but exposes remaining visual requirements:

- Column entry labels still use a tiny core-style font beside scaled modern
  fields. Review ownership/inheritance of its label render table.
- IconButton retains cramped two-pixel margins and square framing; modern
  defaults need shared rounded button painting, focus and scaled padding.
- Gallery icon examples still use fixed 16-pixel hand-drawn bitmap assets, and
  IconLabel's sample does not supply an icon. Refresh the examples with proper
  themed/scaled artwork and clear lifetime ownership.
- A stretched ToggleButton keeps its caption centered while its indicator stays
  at the leading edge. Modern default alignment must keep the caption adjacent
  to its indicator while honoring explicit application alignment.

These findings remain open. Geometry containment alone is not visual acceptance.

Column investigation: its generated labels receive `XmNrenderTable` from the
Column's default entry table as an explicit constructor argument. That hides
the difference between a toolkit default and an application font. Appearance
font-provider tracking currently recognizes the standard label/button/text
aliases, but not Column's `defaultEntryLabelFontList/RenderTable` aliases.
The next correction must preserve explicit Column/entry fonts while allowing
ordinary generated labels to inherit modern typography at construction and
through live changes; changing only gallery label sizes would mask the defect.


The first core-font container run found Disclosure two pixels shorter than its
preferred height after classic 1.5x changed to modern light 2x. Its arrow queried
the still-committed old profile during the transaction, while other child metrics
used the target profile. Arrow geometry queries now use the internal target
profile/density context. The core-font 96-case container fixture passes with
that correction; the preceding uncorrected run and its transition context are
retained. Arrow and full-gallery requalification are running against this change.


## Column generated-label typography checkpoint

Column previously forwarded its default BulletinBoard font table as an explicit
renderTable argument when constructing each label. This incorrectly transferred
font ownership to the child and prevented modern typography from scaling it.
Generated labels now use Label's default-font trait unless an entry supplies its
own font. Appearance provider tracking recognizes both Column default-entry font
aliases, preserving application-supplied parent defaults.

The extended composite-font regression passes in Xft/SVG and core-font/non-SVG
builds (`column-font-startup-1`): newly created labels match ordinary labels at
1.5x, continue matching at 2x after a profile change, and retain explicit parent
and per-entry font choices. Verified gallery processes were captured in modern
light, dark and 1.5x light under `gallery-column-font-review`; the scaled window
was inspected and entry captions now match field typography without clipping.
The subsequent `column-font-complete-1` native fixture passes in both builds.
It also covers dynamic per-entry font assignment/reset, initial explicit-entry
reset, classic restoration, equal-valued explicit parent assignments, and both
Xrm default-entry font aliases. Resetting an entry to NULL now restores default
font provenance and reapplies the current profile immediately; changing the
parent default is propagated even when its table is equal to the stored one.
Both `column-font-final-` suites passed: composite font tests, 96 container
configurations, appearance parity/density, regular gallery integration and all
376 live gallery transitions per build. The durable summaries under
`column-font-validation/roles-*` retain source/output fingerprints and step
results. This closes the Column checkpoint, not the overall visual overhaul.

The preceding container target-context change also passed both complete
`container-target-final-` suites, including native container/arrow checks,
appearance parity/density, regular gallery and all live gallery transitions.


## IconButton shared modern painting and spacing checkpoint

IconButton now uses the shared rounded button face and focus-band raster in
modern light/dark profiles. Text and pixmaps retain their native drawing paths;
pressed and disabled state use the existing foreground and arm-color ownership.
Classic and high-contrast profiles retain the existing shadow renderer.
Horizontal margin, vertical margin and icon/text padding participate in the
appearance transaction with comfortable defaults 12/6/8 and compact defaults
8/3/6 logical pixels. Application overrides remain authoritative.

The extended native button fixture passes in both Xft/SVG and core-font/non-SVG
builds (`icon-button-modern-3`): 72 existing state configurations plus IconButton
coverage in 12 scale/profile/density combinations. Checks include connected,
mirrored focus pixels, expose stability, pressed/disabled faces, native pixmap
preservation, keyboard activation, explicit margins and classic restoration.
The first two attempts exposed test-harness mistakes (a synthetic zero-time
mouse event took the double-click path, and startup-only density configuration
was used for a live change); the corrected test sends a keyboard event and
uses XmApplyAppearanceDensity.

Six actual gallery windows were captured in `gallery-icon-button-review` with
verified process/library fingerprints. Scaled light IconBox and dark IconButton
were inspected: margins and rounded frames are coherent. The demo still uses
fixed-size tiny bitmap assets in IconBox and supplies no icon in the standalone
IconButton example. Those demo-content defects remain open, as does explicit
undersized-pixmap clipping coverage. Broader integration qualification follows.


### IconButton clipping and integration follow-up

The first broader integration attempt stopped at IconBox's 300% natural-size
case: three explicit columns requested 1459 pixels inside a fixed 1400-pixel
DrawingArea. This fixture tests unconstrained natural growth, so its parent now
provides 2400x1600 pixels; it still checks actual/preferred geometry and child
containment in every configuration. A suspected partial geometry-reply defect
was separately investigated with a poisoned unspecified border field. Both the
original and revised code passed because Xt normalizes the reply; the proposed
IconBox implementation change and redundant probe were removed.

An oversized-icon pixel regression did fail against the preceding library:
a 64x64 bitmap overwrote the frame of a 31x29 IconButton with zero margins.
Rounded drawing now emits its mirrored spans through a shared internal helper;
painting and clip-region creation consume exactly those same spans. IconButton
clips pixmaps, disabled stippling and text to the inner face without repainting
over application content. Negative text-extent calculations are clamped.
The native button test checks preserved border pixels, corner background and
connected focus after drawing the oversized bitmap. Both `icon-clipping-final-`
suites passed: native buttons, 96 container configurations, 504 rounding cases,
appearance parity/density, regular gallery and all 376 live transitions per
build. Durable summaries in `icon-clipping-validation/roles-*` retain the exact
source/output fingerprints and validation steps. Gallery asset refresh remains
open; this does not complete the visual overhaul.


## Modern toggle caption alignment checkpoint

Modern ToggleButton and ToggleButtonGadget captions now align at the beginning
beside their checkbox/radio indicator, including RTL. Indicator-less toggles
retain their original centered default. Alignment is a typed appearance
property, with explicit constructor and SetValues choices preserved. Indicator
visibility changes reapply owned defaults immediately, and classic restores the
original default.

The extended `toggle-alignment-3` native fixture passes all 128 configurations
in both builds, retaining state, disabled and keyboard tests and adding actual
caption-coordinate checks in Form-stretched controls, live indicator changes,
classic restoration and explicit alignment ownership. The first implementation
used an invalid converter spelling; it now uses the registered
`alignment_beginning` value. The position fixture uses Form attachments because
ordinary recomputeSize behavior may replace an initial width while fonts change.
Six verified gallery windows were captured in `gallery-toggle-alignment-review`.
Scaled light and dark FormSection windows were inspected: checkbox captions are
adjacent to their indicators. Section header/field-label indentation remains
visually inconsistent with the input edges and is a separate open layout issue.
Both `toggle-alignment-final-` suites subsequently passed container geometry,
appearance parity/density, regular gallery and all 376 live transitions per
build. Durable records are in `toggle-alignment-validation/roles-*`.


## FormSection layout and gallery construction checkpoint

The old gallery nested another padded RowColumn and a second heading inside
FormSection's content, leaving the real header empty. That RowColumn also
copied the checkbox caption margin onto adjacent field labels. The demo now
sets formSectionHeader and adds controls directly to the owned content host.
FormSection's host disables cross-control label-margin adjustment and adds no
second outer padding. Header/footer roles come from the toolkit. Modern outer
padding is 12 logical pixels (compact 8), and section/content spacing is 8
(compact 6). Explicit resource values retain ownership; classic defaults restore.
Compound XmString copies now preserve tags and formatting instead of flattening
through plain text.

Both builds passed the 96-container fixture with unchanged label margins after
adding toggles. The expanded `form-section-metrics-native` fixture also checks
startup/live scale metrics, classic restoration and explicit margin/spacing
retention. The FormSection ownership/accessibility test passed 13 checks per
build, including compound-string preservation. Six source-excerpt parser tests
passed after regenerating the actual demo excerpts. Three verified gallery
windows are recorded in `gallery-form-section-layout-review`; the scaled light
window was inspected and header/field-label text now shares a coherent leading
edge next to the input boundary. Broader appearance/gallery checks follow.

Completed temporary native test executables from prior checkpoints were removed
by exact task directories after checking active executable paths; receipts and
logs remain. The cleanup record is `visual-checkpoint-executable-cleanup.json`.


## Shared action artwork and gallery ownership

Seven named action images now use shared maintained geometry for SVG artwork
and monochrome fallback outlines: folder/open, document, save, print, undo,
picture and archive. The gallery uses them in Toolbar, IconBox, IconButton,
IconLabel and Container examples. Each binding owns its image-cache references,
reacquires images after owner colour/scale changes, and cancels pending refresh
work before destruction. Source examples use those same bindings.

The native action-image fixture passes 63 cases in each of the Xft/SVG and
core-font/non-SVG builds: four scales, both colour directions, retained cache
references and explicit installed-image precedence for every name. The regular
gallery integration suite passes in both builds. Fifteen verified gallery
windows were captured; inspected light, dark and scaled examples show the new
artwork with matching backgrounds and no clipped images. The complete live-transition
suite also passes all 376 transactions in each build.

The gallery integration run reports two body Form edge-synchronization warnings.
They also occur in the earlier FormSection qualification; the correction and
stronger acceptance check are recorded below.
The built-in dialog-icons page refresh is addressed in the later checkpoint
below.


## Gallery navigation constraint transaction

The compact navigation page used to attach the sidebar to both Form edges while
the workspace was still managed to its right. Native debugger traces reproduced
two 10,000-iteration edge-synchronization failures at the constraint SetValues
call during responsive navigation expansion. The gallery now unmanages the
workspace before changing those constraints, and manages it again only after
restoring the sidebar's ordinary attachment. This fixes the demo's contradictory
intermediate layout without changing Form's solver or suppressing warnings.

The integration runner now treats this Form diagnostic as failure, including
when the application itself returns success. Responsive checks cover classic,
modern light and modern dark at 1x, 1.5x and 2x; each checks navigation expansion,
selection, keyboard and accessibility activation, containment and restoration
of the user's desktop collapse preference.

Both Xft/SVG and core-font/non-SVG builds pass the full ordinary gallery suite
with all nine responsive combinations and no Form edge-synchronization warnings.
Build identities and before/after traces are retained in the
`gallery-form-transition-validation` evidence checkpoint.


## Built-in dialog icon page refresh

The gallery's dialog-symbol page now binds each image to its actual Label
owner. The binding copies the selected image name, discovers its logical
dimensions, reacquires scaled pixmaps when the owner palette or DPI changes,
and retires old cache references. Classic bitmap fallback images retain their
native aspect ratio. Unsupported extra symbols still report their SVG
requirement in non-SVG builds. The page no longer retains one-shot images
flattened onto the startup palette.

Pixel validation now also checks that the owner resource contains the pixmap
held by its binding. The flow-page fixture adds four live transitions of the
existing widgets to its resize/content checks. All 90 cases pass in each of
the SVG and non-SVG builds. Three verified gallery windows were captured;
inspected dark and 1.5x light views show all nine vector icons, matching
backgrounds and readable captions. The broader gallery suite and all 376 live appearance transitions pass in
each build. The overall visual overhaul remains unfinished.


## Badge natural geometry and modern outline

The fixed-size Badge example masked a toolkit default-size defect. The extended
native label fixture reproduced an allocation smaller than its measured
two-line label. Badge now uses the displayed count or complete compound label
for initial and live geometry, scales its padding, and tracks independent
automatic width/height ownership. Explicit dimensions remain authoritative;
setting an axis to zero restores natural sizing. The gallery no longer supplies
a fixed Badge width or height. Its private flags join the pending ABI-9 cohort.

Modern Badge outlines use the common mirrored rounded-span renderer, with
centered clipped text. The native count transition initially exposed a retained
text clip in the drawing GC, which erased part of the next border. The renderer
now resets transient clipping before and after drawing and refreshes its
foreground/background from current resources. Classic keeps its rectangular
outline.

All 96 Prompt/IconLabel/Badge geometry configurations pass in both builds,
including large-scale and multiline measurement, explicit-dimension ownership,
RTL, count/label transitions, live foreground changes and mirrored badge-border
pixels. Three verified gallery captures were produced; the inspected 1.5x light
window shows a compact centered count. The broader gallery suite, all 376 live
appearance transitions, and the five-check Badge accessibility fixture pass
in each build. Evidence is retained in `badge-natural-validation`.


## Chip default-size reproduction (superseded below)

The next native control audit reproduces a similar defect in Chip: an 8x8
allocation for a two-line label whose preferred size is 123x50. The gallery
currently masks this with a fixed 160x36 example. The new chip-geometry fixture
is a failing reproduction, not an accepted fix. Further inspection found
unscaled drawing offsets and a removal hit target fixed to the right edge.
Natural sizing, shared draw/hit geometry, RTL and modern state rendering must
be corrected together before removing the demo dimensions.


## Chip measured layout, states and removal action

Chip now measures its compound label and optional pixmap and includes scaled
padding, focus space and the removal slot in natural geometry. Automatic axes
track content, density, DPI and profile changes; explicit dimensions remain
application-owned. The gallery's fixed 160x36 size is removed. Geometry uses
the target appearance transaction so classic restoration does not retain the
old modern minimum height.

Rendering and hit testing consume one bounded layout. The leading icon and
trailing removal target mirror in RTL. Colour pixmaps and depth-one bitmaps
are clipped to their allocated icon bounds. The shared mirrored-span renderer
paints the capsule and focus ring, while modern profiles add hover and pressed
feedback. Transient text clips are cleared before reusing the GC.

Pointer input uses button 1 and commits only on release over the original
body/removal target. Disabled state cancels an outstanding press. Callbacks
may destroy the widget because all state changes and painting precede dispatch.
The accessibility adapter exposes named activate/remove actions and rejects
removal when the chip is disabled or no longer removable. Private automatic
size and pointer-state fields belong to the pending ABI-9 cohort.

Both builds pass 32 native geometry configurations including live density/
profile/scale changes, two parent managers, RTL and both pixmap depths; 16
pointer/keyboard/focus configurations; and 16 accessibility/API checks. Three
verified gallery windows were captured; the inspected scaled-light view shows
a naturally sized, readable pill and removal mark. The full gallery suite and
all 376 live appearance transitions pass in both configurations. Records are
retained in `chip-modern-validation`.


## Avatar circular rendering checkpoint

The native image fixture reproduced top-left cropping of an oversized image.
Avatar now center-crops images, uses XCopyPlane for depth-one bitmaps, and
shares one symmetric outline and circular clip between images and initials.
An offscreen text surface gives Xft and core-font initials the same final
clip; oversized compound strings cannot paint through the outside corners.
GC foreground/background and sensitivity colours update on live changes.
Copied initials are freed on replacement and destruction. The unused fixed
core-font allocation is removed from the pending ABI-9 private record.

Both configurations pass 45 native rendering cases across classic/light/dark,
including centered crop, bitmap depth, live colour changes, long initials,
non-square allocations and mirrored outlines at ten sizes from 1 to 64 pixels.
The nine-check accessibility fixture passes in each configuration. Three
verified gallery windows were captured and the scaled-light window inspected.
The full gallery suite and all 376 live transitions pass in both builds. Evidence is retained in
`avatar-rendering-validation`. Automatic size ownership, density/DPI defaults
and removal of the gallery's explicit dimensions remain the next Avatar work;
this rendering checkpoint does not qualify those unfinished behaviours.


## Avatar natural sizing and appearance ownership

The native geometry fixture reproduced an 8x8 actual allocation with a 32x32
preferred size before the fix. Avatar now initializes automatic axes from the
original creation request and updates them when avatarSize changes. Explicit
width and height are independently authoritative; zero restores automatic
ownership for that axis. QueryGeometry follows the standard Xt reply contract.

The appearance system owns modern default avatarSize just like other toolkit
metrics: 48 logical pixels in comfortable density and 40 in compact density,
scaled on startup and live changes. Explicit avatarSize values are preserved;
classic restores the original default. The gallery no longer supplies width
or height, and its generated source tabs reflect the actual constructor.

Both configurations pass 16 native geometry configurations across two parent
managers, two modern palettes and four scales. Each exercises live density,
scale, classic restoration, explicit avatarSize and axis ownership, zero resets,
and query replies. The 45 rendering cases and nine accessibility checks also
pass on these builds. Three fresh verified gallery windows were captured; the
scaled-light view shows the larger, correctly scaled natural marker. The full gallery suite and all 376 live transitions pass in both builds.
Durable focused and full-suite evidence is
in `avatar-natural-validation`; the earlier rendering-only full suites passed.


## Divider measured layout and line rendering

A native reproduction found an 8x8 allocation and a 17-pixel preferred height
for a two-line label measuring 40 pixels high. Divider now shares measured,
saturated natural geometry between initialization, live changes and queries.
Width/height ownership is independent; zero restores an automatic axis.
Modern default spacing follows density and scale while explicit spacing is
preserved. The gallery's fixed 360x40 dimensions have been removed.

Labeled and plain lines now honor the same thickness. Line segments are
bounded to the allocation and omitted when there is no room beside the label;
text is clipped, and palette/sensitivity changes refresh drawing colours.
The modern line uses bottomShadowColor and the label uses foreground.
The owned label is freed on destruction, and a failed font replacement keeps
the previous owned render table.

Both builds pass 32 native configurations across two parent managers, both
modern palettes, four scales and RTL/LTR. These include multiline text,
maximum resource bounds, exact line pixels, live colours/density/scale,
explicit spacing and geometry, classic restoration, vertical and tiny layouts.
The nine-check IconLabel/Divider accessibility fixture passes in both builds.
Three verified gallery captures were produced; the inspected scaled-light
window has a readable title with balanced, fine line segments and no custom
size. The full gallery suite and all 376 live transitions pass in both builds;
evidence is retained in
`divider-natural-validation`. The generic gallery interaction instructions
still incorrectly address passive controls such as Divider and Avatar and
need a separate presentation-copy correction.


## Gallery guidance for passive examples

The example page now describes the constructed demo as a passive preview,
interactive example, popup window, infrastructure class or unavailable optional
component. Passive examples direct the reader to Theme/Scale comparisons and
hide their unused callback feedback row; the preview reclaims that space.
Interactive selections restore the row. Compact descriptions preserve the same
meaning, and the native component matrix checks feedback visibility while
switching through the catalogue.

Both gallery builds completed with the changed gallery.c recompiled and the
unchanged toolkit reused. Six verified windows were captured across Divider
and PushButton in light, dark and scaled light. The inspected passive view has
no misleading interaction status; the interactive view retains feedback and
its normal/disabled/default examples. The full gallery suite and all 376 live transitions pass in both builds.
Records are retained in `gallery-guidance-validation`.
The prior Divider checkpoint's full ordinary and 376-transition suites passed
in both configurations and their records are retained locally.


## NumberBox native editing reproduction and implementation contract

The new native `run-numberbox-editing.py` fixture demonstrates that Home followed
by digit 1 and Return changes committed 3 to 31 instead of 13. Current NumberBox
is a Primitive with a private append-only text buffer; it does not have a caret
or selection. Its initial dimensions are also masked by inherited placeholders,
its query is fixed at 140x32, and its drawn arrows/hit target use unscaled
right-side constants. This is an open defect, not an accepted fix.

The replacement must compose the existing TextField and ArrowButton controls
under a manager, so native selection, caret navigation, clipboard operations,
font rendering, focus and modern arrow rendering share toolkit implementations.
Preserve bounded-integer range/step/wrap semantics and the distinction between
tentative text and committed values. Return commits; Escape restores the last
committed value; Up/Down/PageUp/PageDown step it. Invalid text stays visible,
reports through the invalid callback and does not silently change the value.
Callbacks must permit destruction without subsequent widget access.

Automatic dimensions must be measured from range/text/font and scaled child
geometry, preserve independent explicit axes, and negotiate without reentry
loops. RTL places the spin column on the leading edge. Parent profile/font,
sensitivity and foreground changes must reach children without taking ownership
of explicit child overrides. Focus traversal should enter the editor; document
that change and the private-record ABI change. Remove the gallery size override
only after the native defaults are qualified. Replace obsolete source guards
for custom text/polygon rendering with native behavioural coverage; do not keep
the old custom editor as a parallel fallback.

Acceptance requires the editing fixture across light/dark and four scales,
clipboard/selection and invalid/revert paths, disabled input, arrow-button
release/cancel behavior, RTL, large fonts, constrained allocations, live
profile/density/scale changes, explicit resource ownership, accessibility and
destruction from callbacks. Fresh gallery windows and the full gallery/live
matrix must then be checked. The current failing reproduction is retained at
`roles-typography/numberbox-editing-before` on the native test host.


## NumberBox native editor implementation checkpoint

NumberBox now derives from XmManager and composes a native TextField, two
ArrowButtons and an inline validation label. The previous custom buffer,
GCs, polygon arrows and append-only key handler are removed. The editor owns
caret movement, selection and text rendering. NumberBox intercepts numeric
commit/revert/step keys and preserves its normalized integer API and callbacks.
Invalid commits retain text, expose the permitted range and leave the committed
value unchanged. Deferred layout is coalesced and canceled during destruction.
`XmNumberBoxGetTextField` returns the borrowed editor; focus traversal enters it.

Both native configurations pass the existing 15-check numeric/accessibility
contract (updated for child focus) and eight light/dark editing configurations
at four scales. The formerly failing Home/insertion case now commits 13 rather
than 31. Three verified gallery windows were captured; the scaled-light window
shows a real editor and modern chevrons. The source guards now cover delegation,
font ownership, bounded child coordinates and text length instead of requiring
the deleted custom drawing implementation. This is not full acceptance.

Remaining NumberBox work: automatic/default and constrained geometry matrix,
font and palette propagation with explicit child ownership, native clipboard/
selection tests, arrow release/cancel states, callback destruction and pending
layout lifecycle, RTL and inline-error readability, removal of the gallery's
explicit dimensions, and the full live appearance matrix. The current layout
and propagation implementation must be revised wherever those tests expose
weaknesses; the focused editing passes do not establish these contracts.
The ordinary gallery integration suite also passes in both configurations.
Evidence is retained in `numberbox-editing-validation`.


## NumberBox geometry, clipboard and activation lifetime checkpoint

Both builds now pass 32 native geometry configurations across two managers,
light/dark, four scales and RTL/LTR. Coverage includes full integer ranges,
live density/scale/profile changes, independent explicit axes and zero resets,
explicit child-font preservation, tiny allocations and full inline-error label
containment. Modern spacing is 4/2 logical pixels by density, and the gallery
no longer supplies fixed NumberBox dimensions. Eight editing configurations
per build now also cover native selection, clipboard copy/cut/paste, invalid
and value callbacks that destroy the composite, and pending-layout destruction.

Commit handling now uses the TextField's normal activation callback, supporting
both keyboard activation and XtCallActionProc. The added direct-action lifetime
case exposed a shared TextField bug: after its callback destroyed a parent,
Activate propagated through freed memory. A native backtrace confirmed the
fault. TextField now watches destruction during callback dispatch, removes its
stack observer when the field survives, and skips parent propagation for both
immediate and deferred destruction. An independent eight-case native fixture
passes in both configurations and verifies unchanged normal default-button
propagation. The isolated patch and fixture were handed to the bug-fix peer for
independent integration/publication. Commit `19a42bd8d` is verified in both
published remote histories. The peer included a source guard but omitted the
native fixture; publication of that fixture and validation against the peer
build have been requested. The native qualification above applies to this
overhaul build.

Three verified gallery windows were captured after removing the size override.
The scaled-light window shows the naturally sized editor and chevrons. Focused
evidence is in `numberbox-natural-validation`; the full gallery and all 376
live transitions passed in both configurations. Remaining NumberBox acceptance includes explicit colour propagation
and child ownership, arrow press/release/cancel states, invalid-state visual
review, and the broader live suite. The full visual-overhaul goal stays open.


## NumberBox arrow interaction and shared gesture cancellation

Native pointer translation tests reproduced a stale press: press increment,
disable the NumberBox, release while disabled, re-enable and release again.
The final release incorrectly incremented the value. ArrowButton retained its
selected flag across effective sensitivity changes. Its SetValues now cancels
that flag when it becomes insensitive, including ancestor changes. Gadget and
repeated-click activation also require a current armed gesture.

Both font configurations pass the expanded NumberBox editing fixture (eight
light/dark/scale configurations), including press-versus-release notification,
release outside, disabled input, cancellation, clamping/wrapping and destruction
from an arrow step callback. Both also pass 192 shared arrow configurations with
local/ancestor cancellation, widget/gadget and repeated-click checks. Evidence
is retained in `numberbox-arrow-validation`. Incremental builds recompiled the
changed arrow sources and relinked the library; the gallery binary was reused
with its verified dynamic library. Explicit colour ownership and invalid-state
visual review remain open; this checkpoint is not full NumberBox acceptance.


## NumberBox explicit foreground inheritance

A new native fixture reproduced missing foreground propagation at construction:
NumberBox had the explicitly supplied colour but its native editor did not.
The appearance engine now records a foreground provider for each NumberBox
child. An explicitly styled provider supplies the colour, while an explicitly
styled child keeps precedence. Default children continue to resolve their own
profile palette; the editor background is independent. Inherited writes retain
appearance ownership, so live transactions do not freeze a previous default.
Equal-valued parent and child setters still establish application ownership.

The fixture covers four profiles and four scales, constructor arguments,
resource-file parent/child colours, equal-valued overrides and repeated live
profile/density/scale changes. Both configurations rebuilt successfully after
the internal-header change (63 and 117 seconds, with dependent sources rebuilt).
Both builds pass 16 colour-ownership, eight editing and 32 geometry
configurations, the ordinary gallery suite and all 376 live transitions.
Evidence is retained in `numberbox-color-validation`.


## Invalid NumberBox gallery layout

Verified invalid-input captures exposed overlap between the inline validation
message and gallery helper text. The field examples positioned siblings using
one initial height; NumberBox's later height increase left the helper unchanged.
TextField, DataField, Text and NumberBox examples now use an ordinary vertical
RowColumn for caption, control and helper. Toolkit layout follows their changing
natural sizes, including theme/font scaling. Source extraction includes this
setup before construction, so the application and component docs show the same
real construction path. The six excerpt tests pass after regeneration.

Three actual gallery windows driven through XTest pointer/key input were
inspected in light, dark and 1.5x light. The validation and helper lines are
separate and legible; captures and executable/library hashes are retained in
`gallery-numberbox-invalid-layout-review`, with the overlap reproduction in
`gallery-numberbox-invalid-review-3`. A component-matrix assertion now checks
invalid/recovered NumberBox layout. The ordinary Xft/SVG gallery suite passes;
the full ordinary gallery suites and all 376 live transitions pass in both
builds, with records in `numberbox-field-layout-validation`. The stale callback
feedback found during this review is corrected in the following checkpoint.


## NumberBox gallery callback feedback

The gallery now subscribes to both NumberBox public callbacks. Rejected input
reports the unchanged committed quantity; successful commits report the new
quantity. Feedback explicitly describes the last event, rather than claiming to
represent tentative text that has not yet been committed. The component matrix
checks both callback counts and exact rejection/recovery messages.

Both gallery executables were rebuilt from the changed source, with the toolkit
library hash unchanged from the fully qualified colour/layout build. Ordinary
gallery integration passes in both builds; six source-excerpt checks pass.
Three actual invalid-input gallery windows were captured with executable/library
identity recorded. The scaled-light window was inspected: inline validation,
helper and rejection feedback are legible and separate. Evidence is retained in
`numberbox-feedback-validation` and `gallery-numberbox-feedback-review`.

The remaining NumberBox review includes destruction from callbacks attached
through its borrowed native editor, beyond the already-qualified NumberBox
value/invalid callbacks. The overall visual overhaul remains open.


## Borrowed-editor callback destruction

A native NumberBox regression reproduced a crash when a callback on the borrowed
editor destroyed the NumberBox during SetValue. The first backtrace reached
XtIsSensitive from TextField's post-callback repaint. A destruction observer
removed that access, but the expanded test still corrupted the heap. Valgrind
then identified an invalid read inside XtCallCallbackList: immediate destruction
freed the compiled list while its multiple-callback dispatch was still running.

TextField activation and SetString value notifications now own a snapshot of
callback records and stop dispatch when immediate or deferred destruction begins.
NumberBox formatting observes composite destruction and returns that result to
its setters, step and revert paths before further access. Both builds pass 16
standalone narrow/wide setter cases, eight activation cases, eight NumberBox
editing configurations (including SetValue/SetRange/Step destruction), 16 colour
and 32 geometry configurations. The full eight-configuration NumberBox editing
fixture passes Valgrind with zero invalid-access errors; undefined-value and
leak checks were disabled. The standalone tests include callbacks before and
after the destructive callback, avoiding the single-callback blind spot.

Evidence is in `numberbox-editor-lifetime-validation`; full gallery qualification
is running. GCC and Clang ASan could not reserve their required address space on
the test host. A local Valgrind runtime and matching libc debug symbols provided
the memory check without installing system packages. Remaining lifetime review
includes editor verification callbacks and multiple callbacks on the NumberBox
itself; this checkpoint is not completion of the full visual-overhaul goal.


## Shared callback dispatch and NumberBox callback chains

The expanded NumberBox fixture proved that later callbacks still ran after an
earlier callback destroyed the control. TextField's snapshot logic now lives in
the private CallbackI resource dispatcher, shared by NumberBox value/invalid
notifications. It preserves application locking and snapshot registration
semantics, and stops delivery on immediate or deferred destruction. The older
non-Xt extension-record callback mechanism is unchanged.

Both builds pass 17 TextField setter/registration cases, eight activation cases,
eight NumberBox editing configurations with multiple callbacks, 16 colour and
32 geometry configurations. Expanded NumberBox editing also passes Valgrind
invalid-access checking with zero errors (undefined-value and leak checks are
disabled). The preceding full gallery/live suite passed in both configurations;
qualification of the shared implementation is running. Evidence is retained in
`numberbox-shared-callback-validation`. The superseding standalone shared patch
was handed to the bug-fix peer along with ownership of the general TextField
modify/motion-verification callback audit; that dependency remains tracked.

The next visual control is Stepper. Source review confirms it still has fixed
72x32 QueryGeometry, unscaled 28-pixel action geometry, hand-drawn separators and
pointer stepping on press (including clicks on the value). The gallery hides
its natural size with 180x48. It needs measured native layout and proper
press/release/cancel behavior before that size override can be removed.

## Stepper native-control checkpoint

Stepper now derives from XmManager and places a native decrement PushButton,
value Label and increment PushButton in one measured row. The previous renderer
painted symbols at positions different from its hit regions and stepped on press,
including clicks on the value. Native children now own hit testing, focus,
pressed/disabled rendering and release activation. The gallery no longer supplies
an explicit size. Measured range capacity keeps the buttons stable as the value
changes; child overrides, automatic axes and RTL use the composite contract.

A native fixture reproduced a separate PushButton defect: pressing, disabling
and re-enabling the control before release revived its armed state. PushButton
and PushButtonGadget now cancel that state when effective sensitivity is lost.
The widget multiclick and gadget activation paths require a live armed gesture.

The final 48-configuration Stepper matrix passed with Xft/SVG and core fonts,
including font overrides, full integer arithmetic and RowColumn parents. The
expanded shared button suite, standalone 24-configuration sensitivity regression,
numeric/accessibility contract, ordinary gallery and 376-case live gallery matrix
also passed in both builds. Actual light, dark and 1.5x gallery captures were
inspected from verified processes. Valgrind completed the 48-configuration native
fixture with zero invalid-access errors; leak and undefined-value checks were
disabled, so this is not a leak or initialization qualification.
The native test waits for asynchronous layout convergence rather than assuming
a single short timer guarantees that Xt idle work has run.

The overall overhaul remains incomplete. Numeric accessibility is addressed in
the following checkpoint. Final Periodic qualification, screenshot refresh and
cohort publication remain open.

## Native numeric accessibility checkpoint

NumberBox and Stepper expose the AT-SPI spin-button role, committed integer,
inclusive bounds and actual configured increment. Named increment/decrement
operations use their native APIs, including wrapping and callbacks; NumberBox
also exposes page stepping. Value writes reject fractional, nonfinite and
out-of-range inputs, respect sensitivity/modal scope, and may safely trigger
widget or bridge destruction. Internal buttons have descriptive accessible names.

The protocol audit also found incorrect public role constants: progress bar,
tree and calendar must be 42, 65 and 5 respectively. The old values identified
popup menu, root pane and font chooser. Canonical role names now live with native
semantics and are shared by the optional D-Bus adapter. Native role tests compare
against the external protocol numbers, including actual widgets, rather than
comparing one toolkit constant to another.

The 24-configuration numeric matrix and existing TreeView semantic matrix pass
in Xft/SVG and core-font/non-SVG builds. The D-Bus adapter also passes a compile
syntax check against the generated headers and installed libdbus headers. Both
current builds disable the transport; this is not a running D-Bus or screen-reader
qualification. Both ordinary and 376-case live gallery matrices pass. The final
24-case numeric matrix includes dynamic full-int ranges and passes Valgrind with
zero invalid-access errors; leak and undefined-value checks were disabled.
Twenty existing accessibility source guards also pass. Validation receipts and
build fingerprints are retained in numeric-accessibility-validation.

The next visual control is ColorPicker. Its current default is a fixed 180x34
Primitive, and the gallery overrides it to 180x48. Source review found that a
pointer press only toggles the alpha byte and R/G/B keys toggle one channel bit;
there is no ordinary colour-selection UI. This needs a real native editing and
selection surface, measured layout and accessible channel/value operations,
including proper alpha visualization, rather than cosmetic changes to the swatch.

## ColorPicker native multi-mode selection checkpoint

The placeholder described above has been replaced by a measured XmManager
composite. Native RGB, HSV, Lab and Pencils modes share the alpha checkerboard,
hex editor, preview/commit state and Apply/Revert controls. HSV is the default.
The gallery only supplies the initial color; it adds no custom sizing or styling.

HSV provides a saturation/value field with a hue slider. Lab provides an a*/b*
field with lightness control, D50/Bradford conversion, muted out-of-sRGB hatching
and an explicit clipping notice. The requested Lab coordinates remain visible
while preview/hex show the actual bounded sRGB output. Pencils provides 24 named,
keyboard-accessible native buttons with scalable pencil artwork. All modes retain
alpha and share a measured width, avoiding horizontal jumps during switching.

Color fields use a cached raster, visual-specific TrueColor packing or an owned
indexed palette, and bounded strip uploads. Pointer dragging and arrow keys
update the preview; disabling the picker cancels an active drag. Numeric model
channels expose semantic slider values in their displayed units, and hidden
channels reject semantic writes. The native editor retains validation, revert,
font ownership, tiny-allocation and callback-destruction behavior.

Validation completed in Xft/SVG and core-font/non-SVG configurations:

- Native editing/geometry/lifetime matrix: 36 configurations in each build.
- Four-mode interaction matrix: 18 profile/scale/RTL configurations per build,
  plus 4,096 exact RGB/HSV/Lab round trips and known D50 reference vectors.
- Indexed-color rendering: the 18-configuration mode matrix on an 8-bit X server.
- Instrumented mode/lifetime checks: all three profiles, four modes each, with
  zero Valgrind invalid-access errors. Leak and undefined-value checks were disabled.
- Ordinary gallery integration and 376 live appearance transitions in both builds.

Actual gallery review then exposed a shared SegmentedControl defect: reactivating
a required selection kept its index but cleared the native toggle and fill.
Restoring that toggle without emitting a new selection callback fixes it in the
library. After this final correction, the 43-case segmented geometry/pixel suite
and the 18-configuration ColorPicker mode suite pass in both builds. The full
integration runs above precede this small selection correction; targeted native
and actual-window checks verify the final correction.

Sixteen final verified gallery windows cover all four modes in light, dark,
1.5x scaled and classic appearances. They were driven through native pointer
input and inspected from the verified executable/library. The ColorPicker docs
image and linked Lab/Pencils captures have per-image provenance; this does not
claim that all other component screenshots have been refreshed. Twenty-four
superseded fixture executables were removed after preserving their records.
Documentation source is updated; rendered documentation has not been rebuilt.

This checkpoint remains part of the pending ABI-9 overhaul cohort. The independent
required-selection fix has been handed to the bug-fix peer for its own primary
branch qualification/publication. Neither that publication nor completion of the
broader visual overhaul is claimed here.

## SegmentedControl assignment and callback ownership checkpoint

Explicit selection is now idempotent regardless of the native allow-clear gesture
policy. Reassigning the current index and clearing an already-empty selection do
not emit callbacks. Native deselection remains available when allow-clear is set.
Selection notification owns an ID snapshot through the full dispatch, including
callbacks that replace the item model. Destruction stops further notification.
Public setters return request admission without reading a destroyed control;
keyboard navigation follows only a surviving current selection after callbacks.
Home and End scan inward from disabled endpoint items.

The new 12-configuration native selection fixture reproduced the old callback-ID
invalidation, then passed classic/light/dark, RTL and clear-policy combinations
in both Xft/SVG and core-font/non-SVG builds. Existing 43-case segmented geometry
and pixel checks, the 18-configuration ColorPicker mode matrix, and the original
public selection fixture also pass in both builds. Valgrind reports zero invalid
accesses across all 12 selection configurations; leak and undefined-value checks
were disabled. Eight existing segmented source guards pass.

Six fresh verified gallery windows cover SegmentedControl and ColorPicker in
light, dark and scaled modes. The selection-only change preserves the validated
resource/geometry implementation byte-for-byte. Test receipts, matching source
fingerprints and captures are retained with the validation records. The independent primary-branch patch and
portable native fixture were handed to the bug-fix peer; publication is not yet
claimed. The full visual-overhaul goal remains open.


### SearchBox callback ownership checkpoint

Search notifications now use the owned callback snapshot helper and stop at
widget destruction. Clear operations observe destruction across inherited
TextField notifications, reject reentrant clear requests while setting text,
and suppress cleared notifications when text verification rejects the edit.
Placeholder compound strings retain rendition information and are released
by the owning widget. The native fixture also fixes its display teardown order.

The regression failed against the previous library when a commit callback
removed the widget but later callbacks still ran. Both Xft/SVG and core-font/
non-SVG builds now pass 22 native checks, including direct and native Escape
callback destruction, callback-list mutation, preserved query snapshots,
validation vetoes and placeholder ownership. Valgrind reports zero invalid
accesses for the fixture (undefined-value and leak checks were disabled).
These changes do not establish completion of SearchBox visual layout or the
AutoSuggestBox popup redesign, which remain outstanding.


### SearchBox decoration geometry checkpoint

SearchBox now inherits native TextField resize and query behavior and reserves
separate measured icon lanes through internal TextField class hooks. Shared
text clipping, display rectangles, caret visibility, selection autoscroll and
preferred width account for those lanes. Empty text paints the actual compound
placeholder. RTL mirrors magnifier/clear placement. Modern SearchBox palette,
border and density defaults now match TextField; the gallery no longer supplies
fixed field width/height. Primary-button release activates clear, with other
buttons and releases outside rejected.

Both Xft/SVG and core-font/non-SVG configurations pass 36 SearchBox layout cases,
32 existing modern-field rendering cases, 17 TextField setter lifetime cases,
8 activation lifetime cases and the 22-check SearchBox contract fixture.
The layout fixture passes Valgrind without invalid accesses (undefined-value
and leak checking disabled). Source bounds guards, six gallery excerpt checks
and the 105-image asset check also pass. Six fresh gallery windows were captured
from verified executable/library paths, covering SearchBox and AutoSuggestBox
in light, dark and 1.5-scale modern profiles; the two normal-light documentation
images retain individual capture provenance. These captures show the new field,
not completion of AutoSuggestBox popup design or the whole visual overhaul.


### AutoSuggestBox sizing, popup and ownership checkpoint

AutoSuggestBox now derives its natural size from its SearchBox child and keeps
explicit axes independent. Its native scrolled-list popup aligns with the
painted field frame, displays at most eight rows, stays within the screen and
chooses above placement when that side has more usable room. Ancestor movement
repositions it; unmapping dismisses it. Initial results wait until the anchor
is viewable. The gallery uses a real case-insensitive provider and current query
generations, with no fixed dimensions.

Suggestion replacement is transactional and count-bounded, preserves compound
labels and owns no-results text. Query/commit/dismiss callbacks use snapshots
and destruction guards. A native ownership regression identified that Xt frees
the search child before the composite's destroy method; destroy observers now
clear child and popup pointers. Single-click selection commits, verification
vetoes suppress commit, and Escape dismisses without erasing the query.

Both font builds pass 12 native theme/scale/RTL configurations plus five callback
destruction paths, the 14-check original suggestion contract and the 22-check
SearchBox contract. Valgrind reports zero invalid accesses for the final native
fixture (undefined-value and leak checks disabled). Three fresh verified gallery
captures cover light, dark and 1.5-scale modern profiles. The screenshot runner
now captures screen pixels within the gallery bounds so overlapping native
popup windows are visible, rather than returning obscured black window pixels.

Remaining interaction work includes focus/outside-interaction dismissal and
cancellation on ancestor sensitivity changes. This checkpoint does not complete
the whole control audit or the broader visual overhaul.


### Suggestion and List sensitivity cancellation checkpoint

AutoSuggestBox latches cancellation when its effective sensitivity is lost,
including parent changes. Commit and show requests are blocked until a zero-delay
notification runs after the SetValues call stack; immediate re-enabling does not
revive the cancelled gesture. Late provider results remain inactive. Destruction
removes the owned timer. Native List now clears pending selection/append state
and drag/scroll timers on loss of sensitivity, preventing a later release from
completing the old gesture while preserving existing selection data.

The pre-fix native regression reproduced an unexpected commit after an immediate
ancestor disable/re-enable. Both final font builds pass 12 AutoSuggest profiles
with 48 cancellation transitions and seven destruction paths, plus 24 standalone
List cases covering all four selection policies, own/parent disable and three
themes. Original AutoSuggest (14 checks) and SearchBox (22 checks) contracts pass
in both builds. Both native fixtures pass Valgrind without invalid accesses
(undefined-value and leak checks disabled). Four superseded fixture executables
were removed after preserving their receipts; final fixtures remain available.

Focus/outside-interaction dismissal remains outstanding. The broader documentation
navigation gate still reports previously unlisted reference pages; the new List
cancellation page is included in navigation. This is not whole-overhaul acceptance.

### AutoSuggest keyboard focus qualification

The popup now uses an override-redirect transient shell with Motif traversal
support. Focus settlement dismisses after leaving the field/popup, preserving
transfers within them. Both configurations pass 12 native profiles, nine
callback/timer destruction paths, 24 List cancellation configurations, the
14-check AutoSuggest contract and 36-check SearchBox contract. Invalid-access
memory checks pass for the native suggestion and List fixtures. Light, dark
and scaled gallery windows were recaptured from the verified build. Outside
clicks that retain keyboard focus still require implementation and qualification.

### AutoSuggest outside application clicks

Outside button presses now dismiss without consuming the original event,
including inert native surfaces and later-created widgets and shells on the
same display. Observation exists only while the popup is open and is removed
on dismissal/destruction. Both builds pass the 12-profile native suite, its
11 earlier destruction paths plus watched-widget and cross-popup teardown,
List24, AutoSuggest14 and SearchBox36. Invalid-access memory checks pass. A
native XTest press on the actual verified gallery heading dismissed the popup
while preserving its query. Events sent to another X client are not intercepted;
leaving the application is handled by focus-loss dismissal.

### Periodic current-build overview review

The Xft/SVG Periodic executable, Mrm, UIL and UID were rebuilt against current
source. Native checks pass light, dark, scaled, high contrast, compact, classic,
historical and explicit-override modes plus startup failures. Overview captures
for classic, modern light, 1.5×, dark and compact are refreshed in the tabbed
reference page. The duplicate hard-coded popup instruction border is removed
from toolkit-default modes and retained in historical resources. Full menu,
dialog, alternate notebook-page, large-font and state matrices remain open.

### Periodic notebook editor and mnemonic memory qualification

The Toggles editor uses native Column entry labels and follows its scrollable
viewport width. Native geometry checks require all nine selectors to share a
left edge and fit their horizontal viewport; all eight appearance modes and
startup error cases pass in the Xft/SVG build. Actual light/dark windows were
recaptured from the verified executable and mapped library, with provenance
in the Periodic screenshot manifest and compact tabs in its reference page.

Valgrind initially found an eight-byte invalid read in menu mnemonic underline
matching: compact XmStrings were probed as padded entry records. Integrating
peer commit e0df14bbd replaces that probe with a compile-time layout comparison.
The actual Periodic native fixture now passes through rendering, notebook
switching, resizing and teardown with zero invalid-access errors. Undefined-value
and leak checks were disabled; this is not a claim of complete memory safety.
The related immediate-payload-copy correction still needs prerequisite review
before integration. Full alternate-page/dialog/menu and large-font acceptance,
core-font qualification of this application change and final publication remain
open. Documentation source/assets are updated; rendered HTML was not rebuilt.

### Periodic dialog palette review

Eighteen actual Periodic dialog windows (nine types, light and dark) were opened
through native pointer interaction and captured from the verified process. The
review exposed stale gray rectangles behind default SVG message symbols. Their
pixmaps were created before final palette application and were not regenerated
when the MessageBox colors changed.

MessageBox now reacquires internally owned symbols on relevant palette changes,
while preserving explicit custom pixmaps. The new pixel regression fails before
the fix and passes afterward for all five default SVG symbols; the non-SVG
bitmap fallback contract also passes. Ten subsequent actual message-dialog
captures confirm the rectangles are gone in light and dark. The SVG fixture
passes Valgrind invalid-access checking with leak/undefined-value checks disabled.
Periodic's eight appearance modes and startup cases remain green in Xft/SVG.

The core-font library rebuild took 116 seconds because its generated Makefile
and string definitions were refreshed, causing broad recompilation; the build
log confirms compilation and linking rather than reuse of an old library.
Current build/source hashes, test logs, memory result and capture provenance are
retained in the task's dialog-background-validation.json. The superseded failing
fixture executable was removed while retaining its failure log. The independent
fix is handed to the bug-fixing peer for publication; the wider overhaul and
remaining dialog/menu interaction, font and scale matrices are not complete.

### Live MessageBox symbol scaling

A further native regression reproduced an existing dialog retaining its 40-by-40
symbol after the display scale became 150%. MessageBox now observes the toolkit
scale notification, reacquires internally owned artwork at the new raster size,
and submits it to the symbol label's ordinary geometry path. Custom pixmaps are
unchanged; the observer is removed at destruction and no private fields are added.

All five default SVG symbols pass 100/150/200/100% transitions. Realized light
and dark information dialogs verify symbol-label bounds through those changes.
The core-font/non-SVG contract passes too. The SVG native fixture completes
Valgrind with zero invalid accesses (leaks/undefined-value checking disabled).
Actual current-build Periodic Information dialogs were recaptured at 100% in
both themes and their documentation provenance updated; the live-scale geometry
claim comes from the native fixture, not from those static captures.

The task's dialog-live-scale-validation.json retains build/source hashes, native
results, memory output and capture receipts. Four superseded fixture executables
were removed with logs preserved. The independent fix was handed to the bug peer;
publication and the overall overhaul acceptance remain outstanding.

### Prompt content geometry qualification

Current-source review confirmed the earlier gallery sizing repair is already
absent for Prompt; the remaining static geometry test was stale and still
required fixed-size code. It is replaced by a native fixture measuring compound
strings and actual rendered pixels in two themes, three scales and both layout
directions. The fixture exposed text painting into padding under constraint.

Prompt now aligns against its allocated padded content width, so right-to-left
text follows the right edge when the widget grows, and clips drawing to the
content rectangle on all sides. Natural and explicit axis ownership, multiline
measurement and zero-to-automatic reset pass without host repairs. Both Xft/SVG
and core-font builds pass all 12 configurations and the existing nine-check
visibility/accessibility fixture. The latter's teardown now saves its Display
before destroying the shell rather than dereferencing a destroyed widget.
The new Xft/SVG fixture passes Valgrind invalid-access checking; leak and
undefined-value checking were disabled.

Fresh actual gallery light/dark/150% captures were inspected and the Prompt
reference image/provenance refreshed. Build, source, test and memory evidence is
retained in prompt-layout-validation.json; the failing baseline executable was
removed with its log preserved. The independent rendering fix is handed to the
bug peer for publication. This does not finish other controls' geometry review,
the full screenshot/state matrix, or the overall overhaul acceptance.

### IconLabel constrained rendering

Native pixel checks exposed the analogous constrained-allocation defect in
IconLabel: pixmap copies and compound text could draw through the reserved
padding. Icon copies now intersect their source/destination rectangles against
the allocated content, and text receives a separate clipped area beside the
icon. Beginning alignment follows the available area in both directions.
No new private fields or host-side geometry repairs are involved.

Both font builds pass 36 combinations of content type (text, icon, combined),
modern theme, scale and direction, plus the existing nine-check accessibility
contract. Tests cover actual ink bounds, explicit geometry ownership, multiline
text and automatic-size reset. The Xft native suite completes Valgrind with
zero invalid accesses; leaks and undefined-value checking remain disabled.
Actual light/dark/150% gallery windows were inspected and the reference image
and provenance updated. Evidence is retained in iconlabel-layout-validation.json;
the superseded baseline executable was removed with its failure log preserved.
The independent drawing fix has been handed to the bug peer for publication.
The remaining cross-control, interaction, documentation-build and publication
requirements remain open.

### Current gallery-wide qualification and remaining switch defect

The current verified Xft/SVG and core-font gallery builds pass navigation,
keyboard/focus restoration, component construction, nine responsive profile/scale
cases and appearance controls. Both also pass all 376 live profile/density/scale
transitions without recreating the selected component. Live runs took 137 and
115 seconds against the recorded 147-second reference. Receipts with exact
build/source identities and logs are retained in gallery-wide-current.
Documentation source gates pass: six excerpt tests, 105 screenshot assets,
475 navigation pages and 2013 explicitly excluded reference pages.

These tests do not establish text/indicator geometry correctness: the live gate
checks transaction state and component containment within its host, not internal
content extents. The explicit-size audit found 32 construction blocks with width
or height arguments. Canvas/viewport content budgets must be distinguished from
fixed dimensions masking defects in content-driven controls.

Actual current gallery captures at 150% expose ToggleSwitch still using tiny
unscaled artwork and a bitmap-font label while the surrounding interface scales.
Its implementation copies the default font without fontList/renderTable resources,
draws a fixed 44-by-24 track, and initializes natural width using an older
36-pixel track formula. The gallery supplies 220-by-44 dimensions. This is an
unresolved toolkit defect, not accepted modern rendering.

The next switch work must unify measured natural size, render-table ownership,
DPI/density metrics, RTL placement, constrained drawing, focus/disabled/selected
states and normal pointer/keyboard behavior, then remove the gallery dimensions.
The action's post-callback redraw was separately handed to the bug peer for
lifetime review. Current baseline captures and identities remain in
gallery-switch-baseline-current; publication and full-overhaul acceptance remain
open.

### ToggleSwitch redesign qualification in progress

The switch now uses inherited application fonts, scaled track/knob metrics,
independent natural dimensions, RTL leading placement and native press/release
cancellation. The gallery no longer supplies fixed switch dimensions. Both
Xft/SVG and core-font/non-SVG builds pass the expanded public contract probe,
including release outside, sensitivity cancellation, automatic axis resets and
destruction from an action callback. Three verified gallery windows were captured
in `gallery-switch-redesign-current`; the 150% light capture confirms the label
and track scale together. This is not final acceptance: pixel/state coverage,
allocation-failure coverage, replacement of obsolete source-text guards and
memory qualification remain outstanding before publication.

The subsequent pixel fixture exposed a real vertical asymmetry at 125% scale.
Matching track/knob parity fixes it: both configurations now pass 20 combinations
of theme, scale and state, including RTL mirrors, disabled pixels, one-pixel
allocations and a missing GC. The Xft contract probe reports zero invalid-access
errors under Valgrind (undefined-value and leak checking disabled). Updated
verified gallery captures are in `gallery-switch-centered-current`, with build,
red/green pixel logs and memory evidence in `switch-centered-validation.json`.
Allocation-failure injection and obsolete source guards still need qualification;
this remains an unpublished partial implementation.

The allocation-failure qualification now passes in both builds: seven injected
failures cover label replacement, empty-label reset, font replacement, initial
label/fallback allocation and the owned GC. Valgrind reports zero invalid
accesses on those paths (leaks and undefined values excluded). Five obsolete
source-text guards were removed; their documentation now points to the native
pixel and fault-injection runners. Evidence: `switch-failures-validation.json`.
Final switch acceptance still requires live font/density/scale transitions,
pressed/hover/focus coverage and non-default-colormap contrast review.

Live switch testing found a classic-profile scale defect: existing natural
geometry stayed unchanged while new widgets adopted the new track metrics.
The switch now registers and removes a scale observer that negotiates automatic
sizes through SetValues without overriding explicit axes. Both configurations
pass 30 live/fresh comparisons across three profiles, two densities and five
scale transitions. Both also pass the expanded pixel suite (hover, press,
release outside, focus and focus removal) and all seven allocation failures.
The remaining switch review includes non-default-colormap thumb contrast and
final gallery/documentation integration before publication.

Custom-colormap review found the on-state thumb incorrectly used screen pixel
IDs for black/white. A full 8-bit application palette reproduced a green thumb
where white was intended. Contrast now compares actual colors already owned by
the widget; no color allocation occurs during expose. The indexed-colormap test
passes in both builds, as do the final pixel, live-metric and fault-injection
suites. Gallery integration and the refreshed documentation capture are the
remaining handoff checks for this switch redesign; broader overhaul gates remain.

The final switch gallery integration passed both configurations (21.14 s Xft,
17.42 s core fonts). Three windows from the verified current executable/library
were captured in `gallery-switch-final-qualified`; the light screenshot and its
per-entry binary/library hashes now replace the old fixed-size switch image in
the documentation source. `switch-final-validation.json` retains the current
builds, four native suites, failing colormap baseline and gallery receipts.
Rendered documentation and publication remain part of the overall cohort gates.

### Natural ToggleButton gallery examples and gadget spacing

Removed the gallery's 220×44 overrides from ToggleButton widget and gadget
examples. Actual captures exposed the gadget caption nearly touching its box.
Its natural-width, indicator and text paths used marginHeight horizontally;
RTL ComputeSpace additionally included the gadget parent offset in a relative
coordinate. Corrected those library paths and removed the modern-only indicator
compensation. Both builds pass 18 asymmetric-margin/profile/scale/direction
cases and the existing 128 modern toggle cases. Six verified actual gallery
captures are in `gallery-toggle-spacing-qualified`; documentation images and
source excerpts are refreshed. Evidence is in `toggle-spacing-validation.json`.
The next fixed-size review is RatingControl: its library hard-codes 180×32 and
its query ignores actual count/scale, so removing gallery sizing alone is
insufficient. Rendered docs and the overall publication gates remain open.

### RatingControl natural metrics in progress

Replaced fixed 180×32 natural metrics with count/scale/density-aware geometry
and independent explicit-axis ownership; gallery no longer sets 180×36. Stars
clip to the interior and drawing work is bounded by available pixels, avoiding
Dimension truncation at count 65,536. Concave stars use the appropriate polygon
classification. A live/fresh fixture found classic restoration retaining compact
geometry; explicit appearance geometry participation and classic density rules
fix it. Both builds pass 30 live cases plus the existing public API probe.
Verified 100% light/dark and 150% light captures are in
`gallery-rating-layout-current`; receipts are in `rating-layout-validation.json`.
This is not accepted yet: hit geometry, RTL keys, release cancellation, hover,
callback lifetime, star pixel symmetry, obsolete source guards and final
screenshot/source documentation refresh remain to be completed.

### Chip integration compilation correction (2026-11-01)

The combined tree retained references to a deleted `background_gc` member and
used destruction-aware callback dispatch without its private header. Chip now
includes `CallbackI.h` and updates only its retained GC. Redisplay already sets
that GC's background to the active fill color before drawing the compound label.

Incremental `make -j4 Chip.lo` passed on rpi4 in the existing Xft/SVG build
at 12:03:29–12:03:31 UTC. The tested `Chip.c` SHA-256 is
`d2aca030b77a628b107dfb5da157e4dd5f214354b951903010bf8222ab1259f6`.
The build log is `chip-compile-20261101.log` in the integration workspace on
rpi4. This verifies compilation only; combined-library runtime qualification
and publication remain pending.
