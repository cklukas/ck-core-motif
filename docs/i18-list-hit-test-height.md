# I18List hit-test height arithmetic

I18List motion tracking compares the event Y coordinate with the widget
height in a signed wide type.  The previous cast to `short` could wrap a
large `core.height` before the off-bottom check, causing motion state to be
reported incorrectly for tall list widgets.  The change preserves the
public widget API and only removes that intermediate narrowing.

The source-contract regression is
`tests/automated/check-i18-list-hit-test-height.py`.
