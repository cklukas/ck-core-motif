---
nav_exclude: true
---
# TextField activation callback lifetime

TextField activation callbacks may destroy the field, its parent, or its shell.
The action now observes destruction during callback dispatch and skips parent
propagation after immediate or deferred destruction. Surviving fields retain
normal default-button activation.
