---
nav_exclude: true
---
# Mrm literal fixup failure status

`Urm__FetchLiteral` now propagates failures from its shared literal-value
fixup routine for current-format font lists and other literal types. Previously
those calls could fail while the loader reported success, exposing a partially
fixed serialized value to its callers. Existing successful native and
cross-endian literal conversion is unchanged.

The focused source regression is
`tests/automated/check-mrm-literal-fixup-status.py`.
