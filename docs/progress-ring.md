# ProgressRing

`XmProgressRing` is an additive compact progress presentation. It uses the
same bounded integer range/value contract, `XmNprogress*` resources and
`XmProgressBarCallbackStruct` as `XmProgressBar`, so an application can switch
presentations without translating progress state.

Determinate values draw a clockwise filled arc from the top of a circular
track. Indeterminate state draws a static partial arc and sets the accessible
busy state; it deliberately does not create a timer or process-wide animation
loop. `XmNprogressTextVisible` controls the centered percentage/busy marker,
while `XmNprogressLabelString` places an optional label beside the ring.

The ring derives its default diameter and stroke width from the existing DPI
default contract, while explicit width and height resources remain authoritative.
It exposes the standard progress accessibility role, range and current value,
and retains keyboard focus behavior from `XmPrimitive`.

The installed regression is `tests/automated/run-progress-ring.py`. Existing
`XmProgressBar` and `XmSpinner` APIs and resources are unchanged.
