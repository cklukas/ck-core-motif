# Chip

`XmChip` is an opt-in compact label with an optional leading pixmap and an
optional remove affordance.  The label is copied at creation and through the
normal `XmNchipLabel` resource; the pixmap is borrowed under the ordinary
Motif pixmap lifetime rules.  The chip does not own a collection, attach to a
host widget, launch a command, or remove itself automatically.

Space, Return, keypad Return, or the body invokes `XmNchipActivateCallback`.
When `XmNchipRemovable` is true, Delete, Backspace, or the trailing remove
affordance invokes `XmNchipRemoveCallback`.  The callback receives
`XmCHIP_REASON_REMOVE`; application code owns the decision to destroy or
replace the chip.  If a callback destroys the chip, defer that destruction
until the callback returns so the parent layout manager is not re-entered; the
implementation performs no work after returning from callbacks.

The control has a bounded single-row layout.  Its preferred size includes the
label, optional pixmap, padding, and remove affordance.  It does not wrap,
scroll, create a global overlay, or silently alter a parent’s geometry.
Focus and activation remain ordinary Xt/Motif behavior.  Accessibility
exposes the chip as a button with one activation action; the label is exposed
as its accessible name.  Removal remains a separate application callback, so
applications can choose whether it is represented as a list mutation or a
confirmation workflow.
