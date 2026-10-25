# Mrm argument-list resource descriptors

`Urm__CW_CreateArglist` now skips a missing resource descriptor before reading
its swap flag, group, or conversion type. Valid resource arguments retain their
existing widget-reference and literal resolution behavior.

The source-level regression check is
`tests/automated/check-mrm-createarg-resource-null.py`.
