---
nav_exclude: true
---
# MWM `RenameCommand` strings

`RenameCommand` now bounds its length-prefixed name and validates the remaining
window-list bytes before unpacking them. Truncated or malformed requests are
rejected without tree mutation, and the decoded temporary name is released on
all exits.
