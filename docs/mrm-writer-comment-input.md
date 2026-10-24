# Mrm writer comment input

`UrmCWRSetComment` now rejects a null comment pointer before measuring or
copying the string.  Empty comments continue to clear the widget comment, and
non-empty comments retain the existing UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-comment-input.py`.
