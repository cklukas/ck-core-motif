# ColorSelector string-resource rollback

`XmColorSelector` now copies replacement slider labels, mode labels, and error
strings before releasing the current widget-owned values.  Failed copies keep
the previous resource and avoid updating a child control with a NULL string.

The focused regression is
`tests/automated/check-colors-selector-string-rollback.py`.
