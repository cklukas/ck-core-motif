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
so a later application `XtSetValues` change stops that resource from being
changed by subsequent profile transitions. Widget state, callbacks, focus and
selection state are not reinitialized.

The operation is a UI-thread transaction: callers must not destroy widgets or
run concurrent Xt mutations while it is in progress. Allocation and resource
conversion are preflighted before the first widget is changed; a failed
preflight leaves both widget values and profile selection unchanged.
Callback-driven reentrancy and non-core gadget coverage remain explicit later
work-package gates, as does a complete historical-widget matrix.

WP-02 adds the opt-in `modern-light` profile:

```c
XmSetAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT);
```

It supplies a restrained light palette, a visible two-pixel focus ring, one-
pixel shadows, button/toggle padding, text-field/list spacing and traversal
defaults through ordinary wildcard X resources. The palette is deliberately
documented in the implementation (`#f6f8fb` background, `#202124` text,
`#1a73e8` focus and `#dbeafe` arm/selection). These are defaults, not a color
filter: application resources and explicit widget arguments retain precedence.
The default remains `classic`, and `modern-light` must be selected before
creating the application widgets that should use it.

The palette targets at least 4.5:1 contrast for normal text and 3:1 for the
keyboard-focus indicator against the profile background. The selected and arm
surfaces are intentionally not text colors; their foreground remains the
15.1:1 `#202124`/`#f6f8fb` pair.

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
