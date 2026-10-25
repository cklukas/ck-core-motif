# XPM color helper input guards

The private XPM color helpers now return safely when their screen, color
record, or output symbol array is absent. Valid color-object processing is
unchanged; invalid optional state no longer reaches field assignments or X11
color queries.

The focused regression is `tests/automated/check-xpm-color-input-guards.py`.
