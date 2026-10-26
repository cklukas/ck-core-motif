# Text-source widget attachment

Adding a widget to a shared text source now checks both the shared widget-array
and per-widget highlight-list reallocations.  A failure rolls back the
`numwidgets` increment and leaves the source arrays usable, instead of
dereferencing a null pointer or exposing a partially attached widget.

The focused source contract is checked by
`tests/automated/check-text-source-add-widget.py`.
