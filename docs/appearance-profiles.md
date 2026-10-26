---
nav_exclude: true
---
# Appearance-profile contract

The appearance API has two deliberately separate operations. Selection is
creation-time and keeps the original Xt resource model intact; the live
operation is an explicit UI-thread transaction for an already-created core
widget hierarchy. Neither operation is a desktop-theme bridge or a replacement
for application resources.

WP-01 adds a small, additive selector without replacing Xt resources or
mutating realized widgets:

```c
#include <Xm/Appearance.h>

XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC);
```

The selector is per `Display` and should be called after the display is opened
but before creating application widgets. The selected profile is stored in that
display's existing X resource database. `XmGetAppearanceProfile` reports the selected
profile; an unselected display behaves as `classic`. Unknown enum values are
rejected. Profile selection is immutable for a display: selecting a different
profile after one has been selected fails. Applications that need to change an
existing hierarchy use `XmApplyAppearanceProfile` explicitly instead:

```c
XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK);
```

The live operation updates registered core widgets, then replaces the
display's profile fallback database only after the widget transaction passes
its conversion preflight. It can move between all four profiles, including
back to `classic`. Explicit widget arguments and ordinary application X
resources remain authoritative. Toolkit-owned values are tracked per resource,
so a later application `XtSetValues` assignment stops that resource from being
changed by subsequent profile transitions, even when the assigned value equals
the current default. Widget state, callbacks, focus and
selection state are not reinitialized.

The operation is a UI-thread transaction: callers must not destroy widgets or
run concurrent Xt mutations while it is in progress. Allocation and resource
conversion are preflighted before the first widget is changed; a failed
preflight leaves both widget values and profile selection unchanged.
All profile conversions are retained in a prepared per-widget plan and are
then committed in one pass, so a conversion or staging allocation failure
cannot leave an earlier widget partially updated. The rollback fixture
`tests/automated/run-live-appearance-rollback.py` injects a staging allocation
failure and verifies that the selected profile and realized resources remain
unchanged before a subsequent successful transition.
The transaction is also deliberately non-reentrant. Appearance updates invoke
`XtSetValues`, so a widget class `SetValues` hook can run while the outer
transaction is in progress. A nested `XmApplyAppearanceProfile` or
`XmSetAppearanceProfile` call returns `False`; it cannot interleave a second
resource-database snapshot with the first one. The caller can retry after the
outer call returns. This guard is per display and does not add a global
process-wide serialization requirement. Gadget coverage is part of the same
transaction: primary and secondary resource tables describe cached gadget
resources, and the Xt create hook snapshots values after their `GetValues`
paths are ready. Parent-manager membership prunes gadgets
that have been destroyed without a normal widget destroy callback. A complete
historical-widget matrix remains an explicit later work-package gate; the
representative consumer matrix below is an incremental qualification boundary,
not a claim of full class coverage.

The installed gadget regression is
`tests/automated/run-live-appearance-gadget.py`. It transitions a
`PushButtonGadget`, preserves an explicit foreground, destroys it, verifies
that the next transition prunes its stale state, and creates a replacement
gadget under the active profile.

The live-transition consumer also creates an `XmText` and an `XmTextField`
before realizing the shell, then verifies their committed strings after every
profile transition. Text classes initialize their output/input backends in a
later Motif initialization hook; the appearance registration therefore defers
its first value snapshot for these classes until that backend exists. The
regression is intentionally an installed consumer test, so a null backend or
an accidental text-value reset is a failure rather than an inferred source
property.

The same consumer now constructs a representative historical matrix: Label,
List, ScrollBar, a pulldown-menu item, MessageBox, and Disclosure. It records
their classic backgrounds, then verifies that each toolkit-owned background
participates in modern-light, modern-dark, and high-contrast transitions. This
does not claim complete historical-widget coverage; it ensures the live
transaction is exercised beyond the button/text subset before additional
classes are admitted.

WP-02 adds the opt-in `modern-light` profile:

```c
XmSetAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT);
```

It supplies a restrained light palette, a crisp one-pixel focus ring, two-
pixel Motif bevels, an unframed notebook surface, button/toggle padding, text-field/list spacing and traversal
defaults through ordinary wildcard X resources. The palette is deliberately
documented in the implementation (`#f1f2f4` background, `#202124` text,
`#1a73e8` focus and `#dbeafe` arm/selection). These are defaults, not a color
filter: application resources and explicit widget arguments retain precedence.
The default remains `classic`, and `modern-light` must be selected before
creating the application widgets that should use it.

The palette targets at least 4.5:1 contrast for normal text and 3:1 for the
keyboard-focus indicator against the profile background. The selected and arm
surfaces are intentionally not text colors; their foreground remains the
high-contrast `#202124` foreground on the pale selection surface.

WP-03 adds `high-contrast` for functional accessibility scenarios:

```c
XmSetAppearanceProfile(display, XmAPPEARANCE_HIGH_CONTRAST);
```

It uses black/white text surfaces, a yellow three-pixel focus indicator, blue
selection/arm surfaces, white shadow edges and a light-gray disabled foreground.
The installed fixture checks the calculated contrast ratios, realized widget
colors, focus metrics, keyboard traversal and both explicit and ordinary
application-resource precedence. Pixmaps are not recolored, so application
supplied pixmaps retain their normal resource precedence.

WP-03 also adds the opt-in `modern-dark` profile. It supplies dark surfaces,
light text, visible blue focus, dark selection/arm surfaces and a distinct
disabled foreground through the same immutable Xrm fallback path. Its
installed fixture checks realized colors, disabled state, keyboard traversal,
explicit application precedence and application pixmap identity; see
`docs/modern-dark-profile.md` for the qualified scope and limits.

Profile precedence is unchanged and testable: explicit widget arguments win
over ordinary X resources, and ordinary X resources win over toolkit defaults.
Future profiles must add only documented fallback values and must preserve this
ordering. Applications should still select a profile before creating their
widgets; live transitions are an opt-in update for an existing core-widget
hierarchy, not a substitute for choosing an initial profile.

The installed consumer regression is
`tests/automated/run-appearance-profile.py`. It verifies default/selected
`classic`, rejection of an unknown value, and ordinary-versus-explicit width
precedence against the installed library under Xvfb.

The WP-02 installed visual/interaction regression is
`tests/automated/run-modern-light-profile.py`. It checks actual realized
widget colors, the palette, explicit and ordinary resource precedence, control
metrics, realized geometry and keyboard focus traversal. WP-00 remains the
authoritative actual-window image baseline; WP-02 compares the profile's
realized widget state against that baseline without claiming a desktop-theme
integration.

The WP-03 installed accessibility regression is
`tests/automated/run-high-contrast-profile.py`.

The WP-05.1 installed live-transition regression is
`tests/automated/run-live-appearance.py`. It runs against the installed
library under Xvfb, transitions through modern-light, modern-dark,
high-contrast and classic, and verifies ordinary-versus-explicit foreground
precedence, realized backgrounds, toggle state and callback counts.

## Construction and live-update parity

Construction and live transactions resolve the same immutable Xrm profile
resource database. Class-specific surfaces, label traversal, notebook frame
shadows and border widths therefore have the same defaults whether a widget
is created under a profile or switched into it later. There is no separate
hand-maintained live palette.

Ownership is determined against the original application resource database,
including application-class-qualified resources. An explicit application
value is preserved even if it equals a theme default; equality does not turn
an application resource into a toolkit-owned one. Xt create hooks capture
fully initialized widget/backend/cache values; Xt change hooks record explicit
SetValues arguments, including equal-valued assignments made before the first
live appearance change. Hook lifetime follows the display. Cached secondary
resources participate in both support detection and Xrm resource-class lookup.

The native fixture `tests/automated/run-appearance-parity.py` compares startup
and live values on ten widget/gadget classes under light, dark and high-contrast
profiles, including cached secondary resources, and checks preservation of
equal-valued application overrides.
Run it against a selected out-of-tree build with `--build /absolute/build`
and a fresh `--out /absolute/task/appearance-parity` directory. Its receipt
records source and loaded-library fingerprints and each profile result.

## Density and standard spacing

Density is independent of the selected palette and defaults to
`XmAPPEARANCE_DENSITY_COMFORTABLE`. A modern application can select compact
spacing before constructing its controls, in either order with profile selection:

```c
XmSetAppearanceDensity(display, XmAPPEARANCE_DENSITY_COMPACT);
XmSetAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT);
```

`XmSetAppearanceDensity` accepts a different setting only before registered
controls exist; selecting the current density is idempotent. Use
`XmApplyAppearanceDensity` to update an existing hierarchy. Profile changes
retain the chosen density. Changing density while classic is active stores the
selection for the next modern profile without changing classic widget values.
`XmGetAppearanceDensity` reports the selection. Invalid arguments fail without
changing it. The APIs are UI-thread operations, with the same non-reentrant
preflight transaction and override ownership as profile changes. Both profile
and density setters reject calls during an active appearance transaction.
RowColumn sibling-margin normalization is deferred until every child has its
new resources; otherwise a sibling with old padding could overwrite a child's
new padding midway through a density change. Ownership follows explicit
application assignments, so internal parent-layout adjustments do not
accidentally claim ownership of a child's margins.

Comfortable/compact defaults currently include horizontal button and toggle
margins of 12/8, vertical margins of 4/2, toggle indicators of 18/16, text
margins of 8/6 horizontally and 4/2 vertically, and list spacing of 4/2.
Widgets and their gadget equivalents share these defaults. These resource
values are the density foundation; the full measured-control height, font and
per-widget DPI overhaul remains tracked in `modern-visual-overhaul.md`.

Applications can use the common spacing scale without duplicating constants:

```c
Dimension gap;
if (XmGetAppearanceSpacing(display, XmAPPEARANCE_SPACING_MEDIUM, &gap))
    XtVaSetValues(row, XmNspacing, gap, NULL);
```

`XmAPPEARANCE_SPACING_XS`, `SMALL`, `MEDIUM`, `LARGE` and `XL` represent
4, 8, 12, 16 and 24 logical pixels. The query returns display pixels using the
existing display-scale policy exactly once; do not multiply the result again.
The scale is common to both densities. Invalid display/output/spacing arguments
return `False` and leave the output untouched. Applications that explicitly set
a returned dimension own that resource and should query it again when their
layout handles a scale change.

Run `tests/automated/run-appearance-parity.py --suite density` with the same
`--build` and fresh `--out` arguments as the parity suite. The density fixture
covers both startup selection orders, realized startup/live parity on nine
widget/gadget classes in vertical and horizontal RowColumns, preserved equal-valued X resources and explicit arguments,
subsequent and equal-valued SetValues assignments on widgets and cached gadgets,
profile independence, classic isolation,
reentrancy, text/toggle state and callback preservation, and spacing queries at
1x/1.5x/2x/3x. The allocation-failure fixture also checks density rollback and a
successful retry.

## Remaining overhaul qualification

The full visual overhaul is still in progress. Application-wide wildcard
resources can still lose to more-specific profile patterns during construction;
that precedence case remains a foundation gate. Returning a widget originally
constructed under a modern profile to actual classic defaults also remains
pending. The accepted classic round-trip fixtures start in classic. Neither
these API tests nor spacing-query scale tests establish the full rendered
control, font, DPI or accessibility matrix; see `modern-visual-overhaul.md`.
