# ColorSelector equal-name copy rollback

When a ColorSelector receives a distinct string containing the same color
name, it now copies that string before releasing the old allocation. A failed
copy preserves the existing name instead of replacing it with NULL.

The focused source regression is
`tests/automated/check-color-selector-name-copy-rollback.py`.
