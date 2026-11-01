---
nav_exclude: true
---
# FontSelector argument-list merging

`XmFontSelector` builds its compound interface through 23 internal child and
menu argument-list merges. Each merge now goes through one checked helper that
rejects a `Cardinal` count that would wrap. Existing call sites already return
or roll back when merging fails, so the original cleanup behavior remains in
effect and the subsequent Xt creation count is safe.

The focused source contract is covered by
`tests/automated/check-fontselector-argument-merge.py`.
