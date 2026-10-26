# Modern visual overhaul: implementation and acceptance

This work is in progress. Individual accepted foundation fixes do not establish
completion of the visual overhaul.

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

## Delivery ledger

| Work package | Required result | Status |
| --- | --- | --- |
| Shared appearance | One definition for startup and live styling; explicit resource ownership | Native primary/secondary parity, wildcard precedence and classic restoration accepted for representative classes |
| Density and roles | Public set/get/apply density; normal/title/section/secondary/code/primary/destructive roles; public spacing query; UIL registration | Density API and spacing accepted; roles and UIL pending |
| Typography and geometry | Measured render tables throughout; DPI applied once; natural sizes and live updates | Pending |
| Toggles and buttons | Strong check/radio/mixed states; 44x24 switch; connected segmented groups; coherent buttons | Pending |
| Scrollbars and tracks | Refined arrow buttons, thumb, track, slider, scale and progress treatment | Pending |
| Editing and navigation | Consistent fields, dropdowns, spin controls, tabs, menus, toolbars, lists, trees and grids | Pending |
| Dialogs and glyphs | Toolkit-owned alignment/wrapping/actions; complete glyph family and non-SVG fallback | Pending |
| Gallery simplification | No custom modern fonts/colors/borders/renderers or natural-size repairs; use standard semantic roles | Pending |
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

## Accepted foundation evidence

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

Next foundation gates include role transactions, font ownership and measured
control geometry/DPI. The complete historical-widget, visual and interaction
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
