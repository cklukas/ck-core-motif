---
nav_exclude: true
---

# MWM `f.exec` multibyte command endings

The MWM resource parser retains the established `f.exec` behavior of appending
one trailing `&` when a command does not already end with that delimiter.  The
multibyte parsing path now applies that rule to an empty command and to a valid
command whose last character occupies more than one byte.  Existing commands
that already end in `&` are unchanged.

The multibyte parser keeps no redundant state between character reads, so this
path remains warning-clean under the project’s targeted static analysis.

The focused regression is
`tests/automated/check-mwm-exec-multibyte-delimiter.py`.
