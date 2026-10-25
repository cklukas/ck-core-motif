# RowColumn menu-post validation

`XmRowColumn` now limits passive-grab installation, menu-post duplication and
old-string release to the successful `_XmMapBtnEvent` path. An invalid menu-post
value still emits the existing warning without performing success-only cleanup
through a failed mapping.

The focused source regression is
`tests/automated/check-rowcolumn-menupost.py`; compile `RowColumn.c` with
`-Wmisleading-indentation` enabled to keep the control flow explicit.
