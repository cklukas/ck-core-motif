# ColorSelector color-name allocation

`XmColorSelector` now allocates a replacement color name before swapping it
into the widget state. If `XtNewString` fails, the update returns `False` and
the previous name remains valid; successful color-name display and slider
updates are unchanged.
