---
nav_exclude: true
---
# XmString unparse comparison values

Unparse pattern comparison now treats non-empty components with null values as
non-matches before calling `memcmp`. Malformed compound strings no longer crash
the parser while comparing substitution patterns.

The regression check is
`tests/automated/check-xmstring-unparse-compare.py`.
