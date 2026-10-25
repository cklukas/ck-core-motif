---
nav_exclude: true
---
# GrabShell layout guards

`GrabShell` layout previously accessed `composite.children[0]` unconditionally.
An empty grab shell can exist during construction or teardown, so that access
could dereference a missing child. The shadow and child-border offset was also
formed from unsigned dimensions.

`DoLayout` now returns for an empty shell and computes its offset in signed
arithmetic before deriving the child size. Existing one-child layout behavior,
resources, and the public API are unchanged. The regression
`tests/automated/check-grabshell-layout-guards.py` covers both protections.
