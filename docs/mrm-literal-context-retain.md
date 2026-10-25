---
nav_exclude: true
---
# Mrm literal context retention

After reading a literal, Mrm records the resource context in the caller-owned
context list so nested references can be released together.  If that list
cannot accept the pointer, the loader now releases the context immediately and
returns the append failure instead of leaking it or continuing with incomplete
ownership tracking.

The source-level regression is
`tests/automated/check-mrm-literal-context-retain.py`.
