# Mrm widget-instance input validation

`UrmCreateOrSetWidgetInstance` now rejects a nonzero override count with a
null override array and validates the output/list pointers required by both
create and auto-child dispatch paths before dereferencing them.

The source-level regression is
`tests/automated/check-mrm-widget-instance-input.py`.
