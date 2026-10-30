# Text-source widget attachment

Adding a widget to a shared text source now checks both the shared widget-array
and per-widget highlight-list reallocations.  A failure rolls back the
`numwidgets` increment and leaves the source arrays usable, instead of
dereferencing a null pointer or exposing a partially attached widget.

The attachment path also rejects a missing source, widget, or existing first
widget before it grows the shared array, so teardown inconsistencies fail
without dereferencing invalid state.

The corresponding removal path rejects missing source, widget, or widget-array
state before scanning or mutating the shared list, so teardown cannot turn
removal into a null dereference.

The focused source contract is checked by
`tests/automated/check-text-source-add-widget.py`.
