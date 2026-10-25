# Mrm deferred icon storage bounds

`Urm__CW_CreateArglist` now bounds the deferred icon-image work list before
adding a new entry. Excess icon arguments report the existing failure and are
skipped instead of overwriting the fixed local array; valid icon and bitmap
arguments retain their existing deferred conversion behavior.

The source-level regression check is
`tests/automated/check-mrm-icon-pixargs-bounds.py`.
