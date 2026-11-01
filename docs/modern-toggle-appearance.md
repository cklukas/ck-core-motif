# Modern checkbox and radio appearance

This work is being qualified in the development branch.

Modern light and dark share a native indicator renderer for ToggleButton and
ToggleButtonGadget. The default checkbox uses a rounded outline with a check
when selected and a dash when indeterminate. The default radio uses a circular
outline and selected center dot. Explicit diamond and cross indicators retain
those shapes. Classic and high-contrast use their existing renderer.

The renderer uses the control's resource colors, bounds every drawing operation
inside its allocation, and owns its mutable graphics context. Selected glyph
colors are chosen from the actual foreground/background in the control's
colormap. Widget/gadget state machines, callbacks and keyboard activation stay
in their original classes.

Modern defaults remove the outer toggle bevel, use a two-logical-pixel focus
outline and reserve 18/16 logical pixels for comfortable/compact indicators.
Margins provide natural control heights of at least 34/28 logical pixels and
8/6 logical pixels between the indicator and text. Larger fonts can increase
the natural height. Explicit dimensions, indicator variants, spacing and
unselected colors remain application-owned during live profile changes.

Run `tests/automated/run-modern-toggles.py --prefix INSTALL --out FRESH_DIR`
for native state/geometry and keyboard qualification. The fixture checks
widget/gadget checkboxes and radios, selected/mixed/disabled states, callback
counts, natural height, RTL, tiny allocations, Space activation, explicit
resource preservation and 1/1.5/2/3 display scales in both modern palettes and
densities. Actual application captures are additionally required for visual
acceptance; the fixture is not a substitute for them.


Modern checkbox and radio captions use beginning alignment, placing text beside
the indicator in both layout directions even when a parent stretches the
control. Indicator-less toggles retain their original alignment. Changing
`XmNindicatorOn` updates toolkit-owned alignment immediately. Explicit
`XmNalignment` arguments and resource-database choices remain authoritative;
switching back to classic restores the original alignment for owned controls.

## Gadget margin consistency

ToggleButtonGadget uses `marginWidth` for natural width and both horizontal
origins, while `marginHeight` controls vertical layout. RTL label coordinates
remain relative to the gadget, including gadgets positioned away from the
parent origin. The gallery now uses natural widget/gadget dimensions without
fixed width or height overrides.

`run-toggle-gadget-margins.py --build BUILD --out FRESH_DIR` checks asymmetric
margins, margin updates and nonzero positions across classic/light/dark profiles,
three scales and both directions. This supplements the state/pixel fixture.
