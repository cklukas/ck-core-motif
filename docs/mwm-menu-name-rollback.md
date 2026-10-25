# MWM duplicated menu-name rollback

MWM now allocates duplicated system-menu names before releasing the existing
name. If allocation fails, the duplicate keeps its original name and remains
usable instead of carrying a NULL menu name.

The focused source regression is
`tests/automated/check-mwm-menu-name-rollback.py`.
