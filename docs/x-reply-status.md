# Failed X requests

Several widgets ask the server for the size of a pixmap, the attributes of a
window or the contents of an image. A request can fail: the pixmap or window
was freed, or the application installed an error handler that lets the
connection continue. Xlib then reports the failure through the return value
and does not write its output parameters, so the caller must not use them.

The following places used the outputs without looking at the result, and now
treat a failed request as follows.

| Where | After a failed request |
| --- | --- |
| `XmCascadeButton`, `XmCascadeButtonGadget` cascade pixmap | the cascade has no size |
| `XmDataField` add mode cursor (stipple pixmap and image) | the add mode cursor stays unspecified, as when the stipple pixmap itself is missing |
| `XmIconButton` pixmap | the pixmap has no size and is not cached |
| `XmGetVisibility` sibling windows | a sibling that cannot be queried covers nothing |
| `_XmIsViewable` | a window that cannot be queried is not viewable |
| `XmDragStart` root window event mask | the mask is neither read nor changed |
| Drag receiver information | a receiver whose geometry cannot be read is not a receiver |

## Validation

`tests/automated/run-x-reply-status.py PREFIX RESULTS` compiles
`tests/automated/check-x-reply-status.c` against an installed toolkit built
with the same sanitizers (`--enable-sanitizers`) and runs one process per case
on a virtual X display. The program replaces `XGetGeometry`,
`XGetWindowAttributes` and `XGetImage` with versions that fail on request and,
when they fail, fill the output parameters with values that would do damage if
used (40000 pixel sizes, a viewable window covering everything, event mask
bits). Each case also checks that the failing request was reached, so a case
cannot pass because the code path moved.
