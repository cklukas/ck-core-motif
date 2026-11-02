---
nav_exclude: true
---
# Tooltip popup geometry and lifecycle

Motif creates one tooltip shell and label lazily for each top-level widget.
After the hover delay, the toolkit sets the label text, obtains its preferred
size and positions the complete popup inside the screen. It sizes both the
shell and label **before mapping**. The first visible frame contains the text;
a subtle one-pixel outline surrounds it. A short reveal expands only the
padding around the full text (at most 10 pixels horizontally and six
vertically). The label stays centered and readable in every frame.

Previously the shell mapped at one pixel square and a slide context repeatedly
resized it. A label could have its full text geometry while its mapped shell
still clipped it, producing a slowly growing blank rectangle or resize trails.

The shell and label remain at their final geometry. On servers with X Shape,
the outline and visible padding expand around the already-rendered text.
Without X Shape the complete framed tooltip appears immediately.

The reveal uses one 16 ms Xt timeout at a time and a monotonic 96 ms deadline.
A delayed callback advances directly to the elapsed-time position, without a
catch-up burst. Leaving the control or destroying the label cancels the timeout.

`XmNtoolTipPostDelay` still controls the hover delay. The `XmNtoolTipPostDuration`
timer starts when the complete popup is displayed. Leaving the control dismisses
it. Missing children and popup parents remain safe during posting/unposting.
The slide-context slot in the existing configuration trait is retained for ABI
compatibility; tooltip posting does not allocate a slide context.

`tests/automated/check-tooltip-child-failure-guards.py` checks failure guards.
`tests/automated/run-paint.py` also drives real pointer hovers and checks the
first mapped shell/label dimensions, all four outline edges, text pixels in
every sampled frame, bounded growth, cancellation and settled geometry under
classic and scaled modern appearance. The same first-frame test fails against
the former animated implementation.
