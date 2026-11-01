---
nav_exclude: true
---
# ArrowButton redisplay guards

`XmArrowButton` can receive an expose event while its resources are being
replaced or after a resource allocation has failed.  Redisplay now checks the
shadow and arrow GCs before passing them to the X drawing helpers.  The arrow
geometry helper also returns for zero-sized bounds, which avoids issuing
invalid drawing requests when highlight and shadow margins consume the whole
widget.

Classic rendering remains unchanged when the required resources and
geometry are available.  The superclass expose handler is still invoked so
standard Xt event processing continues for partially initialized widgets.

## Modern arrow bounds

Modern light and dark profiles center the arrow within the content rectangle,
after excluding the control's focus and shadow margins. The glyph is at most
18 logical pixels in comfortable density or 16 in compact density, with at
least four logical pixels of surrounding padding. Small allocations shrink
the glyph; allocations with no remaining content draw no arrow. Display scaling
is applied once. Widget and gadget variants use the same geometry rule.

Modern light and dark profiles use a shared renderer for ArrowButton and
ArrowButtonGadget: a flat rounded frame and an open chevron centered within the
content rectangle. The chevron spans at most 14 logical pixels, with at least
four logical pixels of padding inside the frame. Small allocations shrink the
glyph; allocations without sufficient content draw no arrow. Round line caps
are included in the glyph bounds. Display scaling is applied once.

The frame defaults to one logical pixel. Its color comes from
`XmNbottomShadowColor`; explicit application colors and thicknesses remain
authoritative. Modern palette defaults provide a visible border on both light
and dark surfaces. Pressed controls invert their foreground/background colors;
disabled controls use the subdued border color for the glyph. Each state
transition repaints the full face, while the existing focus rendering remains
outside it. Classic and high-contrast use their existing arrow/bevel renderer.

Modern Forms and BulletinBoards default to no shadow: layout containers should
not introduce a bevel around their children. Explicit shadow resources still
take precedence. Classic and high-contrast profiles retain their own defaults.

`tests/automated/run-arrow-bounds.py` runs an installed-library regression that
reads the actual X pixels for all four directions, both widget types, light and
dark profiles, both densities, and 1×, 1.5×, 2× and 3× scales. It checks enabled
and disabled controls, tiny allocations, and default/explicit container shadows.


`tests/automated/run-modern-arrows.py` checks native frame/glyph pixels and
arm, leave, re-enter and activate behavior in 96 cases per font configuration.
It also verifies live classic/modern transitions and explicit frame thickness.
Pointer-inside state is retained separately from the armed state, so an expose
after dragging outside does not redraw the control as pressed. The regression
forces that repaint for widget and gadget controls and checks the same behavior
in classic and high-contrast profiles.

## Preferred target size

The widget and gadget report the same preferred square target. Modern defaults
are 36 logical pixels in comfortable density and 28 in compact density, enlarged
when explicit focus/frame widths need more room around the chevron. Classic and
high-contrast retain their 15-pixel content extent plus focus and shadow margins.

Unspecified axes follow toolkit metrics during profile, density and scale
changes. Explicit nonzero dimensions remain application-owned; if a parent
subsequently changes that allocation, appearance changes retain the parent's
current size. Setting an axis to zero returns it to natural sizing. Resource-file
dimensions follow the same rules. QueryGeometry reports the natural target even
when a parent or application has constrained the actual allocation.

`tests/automated/run-arrow-sizing.py` exercises these ownership rules across
profiles, densities, scales and both control types. The gallery relies on this
toolkit geometry and supplies only the arrow direction.

## Disabling during a pointer gesture

An arrow press is canceled when either the control or an ancestor becomes
insensitive. Re-enabling does not restore the pending press. A fresh press is
required before a release can activate the arrow, including repeated clicks.
The widget and gadget share this rule in all appearance profiles.

The native modern-arrow fixture checks local and ancestor cancellation for both
control types, including unchanged normal activation after cancellation. The
NumberBox editing fixture dispatches actual pointer translations and covers
release outside, disabled input, cancellation during a press, range boundaries,
wrapping and destruction of the composite from the step callback.
