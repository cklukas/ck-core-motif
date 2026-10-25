# WP-35 compact content and adornment contract

WP-35 is split into independently releasable controls.  Each control owns
only its content and callbacks; placement remains the application's job.  No
control changes the resources or geometry of an existing `XmLabel`,
`XmSeparator`, button, list, or pixmap.

## Chip / Tag / Token

The first implementation target is `XmChip`, a bounded label with optional
leading pixmap and an optional remove action.  The public model is one copied
`XmString` label, one optional pixmap, a Boolean enabled state, and a Boolean
removable state.  The remove callback reports the widget and input event but
does not destroy the widget implicitly.  Applications own the surrounding
collection and call a setter or destroy the chip after the callback.

Chip activation is keyboard-accessible when removable: Space/Return invokes
the normal activation callback and Delete/Backspace invokes removal.  Removal
has a visible non-colour affordance and remains available in high contrast.
There is no implicit list model, drag behavior, truncation menu, or global
overlay.  Tag and Token remain aliases/configurations of this same bounded
contract unless a consumer demonstrates a distinct semantic requirement.

## Badge

`XmBadge` is a non-interactive status presentation with either a copied text
label or a bounded non-negative count.  Counts above the configured maximum
render as `maximum+` and remain available as an accessible value.  Severity
is an enum with informational, success, warning, and error values; severity
never relies on colour alone.  Badges do not attach themselves to a host
widget and never resize or overlay one implicitly.

The additive implementation is documented in `docs/badge.md`; its installed
fixture covers count clamping, label ownership, severity resources and the
read-only accessibility value.

## Icon-and-text label and Divider

An icon-and-text label is an additive configuration of a label-like control:
the icon and text are copied/owned by the control and its direction-aware
layout is measured as one row.  A text Divider is non-interactive, exposes a
copied optional label, and has an explicit orientation and thickness.  Neither
control introduces a new text value or callback contract.

## Avatar

`XmAvatar` accepts an optional pixmap and a copied fallback initials string.
When the pixmap is unavailable or invalid, initials are rendered; fallback is
deterministic and does not load files or URLs.  The control is non-interactive
unless an application composes it with a separate button.  It does not create
a global image cache or modify host geometry.

The additive implementation is documented in `docs/avatar.md`; its installed
fixture covers explicit sizing, unspecified-pixmap fallback, initials
ownership and the read-only accessibility label.

## Work packages and acceptance

1. Implement and test `XmChip` with label/pixmap ownership, remove and
   activation callbacks, keyboard operation, sensitivity, narrow geometry,
   high-contrast rendering, accessibility, and destruction from callbacks.
2. Implement and test `XmBadge` with bounded counts, severity states,
   localization, non-colour semantics, and no implicit host placement.
3. Implement the icon-label and Divider configurations, preserving ordinary
   label/separator behavior and measuring RTL/narrow layouts.
4. Implement `XmAvatar` with pixmap failure and initials fallback tests.
5. Run the unchanged legacy Label, Separator, pixmap and ToggleButton fixtures
   plus every new installed-prefix fixture before releasing the package.

The package is complete only when every accepted control has a documented
public contract, an installed consumer fixture, and teardown/reentrancy
coverage.  Any control without a consumer-ready contract remains deferred.
