# XPM fallback string duplication input

When XPM is built on a platform without `strdup`, its local `xpmstrdup`
implementation now rejects a null source pointer before measuring it and checks
the terminating-byte increment for size overflow. This keeps the fallback
helper consistent with the library's other input-validation paths.

The focused source regression is
`tests/automated/check-xpm-string-duplicate-input.py`.
