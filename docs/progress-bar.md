---
nav_exclude: true
---
# ProgressBar

`XmProgressBar` is an additive, resource-driven progress control. It keeps the
existing Xt/Motif object model intact while providing a small, typed state
contract for determinate and indeterminate feedback.

## State contract

The `XmNprogressMinimum`, `XmNprogressMaximum`, and `XmNprogressValue`
resources are integer values. A maximum below the minimum is normalized to the
minimum, and values outside the resulting range are clamped. The same rules
apply to `XmProgressBarSetRange` and `XmProgressBarSetValue`.

`XmNprogressIndeterminate` selects a busy presentation. It does not create a
worker thread or a global event loop; applications that need animation should
use a separate bounded activity control. `XmNprogressTextVisible` controls the
percentage text, while `XmNprogressLabelString` supplies the descriptive
label.

The `XmNprogressValueChangedCallback` callback receives an
`XmProgressBarCallbackStruct` containing the normalized minimum, maximum,
value, and indeterminate state. The explicit setters accept a `notify` flag,
so applications can batch updates without generating intermediate callbacks.

## Accessibility

The accessibility provider exposes the control as a progress-bar role with a
numeric minimum, maximum, current value, and increment of one. Indeterminate
controls expose the busy state. The label is taken from
`XmNprogressLabelString`; progress controls do not expose actions.

## Geometry and appearance

Explicit `XmNwidth` and `XmNheight` values are honored. When omitted, the
control derives a compact size from the label and its spacing resource. Drawing
uses the current primitive foreground, bottom-shadow, background, render table,
focus state, sensitivity, and layout direction, so theme and high-contrast
profiles remain owned by the application.

## Verification

`tests/automated/run-progress-bar.py` builds and runs
`tests/automated/check-progress-bar.c` under Xvfb. The fixture covers explicit
dimensions, range normalization and clamping, notifications, indeterminate
state, focus/sensitivity, accessibility values and busy state, callback
re-entry, and widget teardown.
