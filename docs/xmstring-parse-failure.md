---
nav_exclude: true
---
# XmString parser failure handling

`XmStringParseText` now tolerates null mapping slots while scanning direction
patterns and aborts immediately when a substitution concatenation fails. This
prevents null mapping dereferences and avoids continuing with a lost partial
result.

The regression check is
`tests/automated/check-xmstring-parse-failure.py`.
