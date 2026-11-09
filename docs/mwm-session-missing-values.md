---
nav_exclude: true
---

# MWM missing session values

CDE session records are input. A malformed record may contain a `-state`,
`-geometry`, `-workspaces`, or `-host` switch without a following value. MWM
now ignores that individual value instead of passing a null token to `strcmp`
or `XParseGeometry`, or reporting a missing string as an allocation failure.
The rest of the record and later records continue to parse normally.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-session-missing-values.py
```
