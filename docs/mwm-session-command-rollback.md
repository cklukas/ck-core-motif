---
nav_exclude: true
---

# MWM session-command parsing rollback

Session restore commands are first tokenized into a temporary vector and then
copied into the persistent session record. The parser now checks vector and
string sizes, frees temporary tokens, and rolls back the destination record if
any element allocation fails. A session record is therefore either empty or
contains a complete command vector, never a partially initialized array.

The successful command format and ordering remain unchanged. The focused
regression check is
`tests/automated/check-mwm-session-command-rollback.py`.
