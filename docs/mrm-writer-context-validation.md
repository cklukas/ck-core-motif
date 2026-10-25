---
nav_exclude: true
---
# Mrm writer context validation

`UrmIdbPutIndexedResource` and `UrmIdbPutRIDResource` now validate their
resource context before reading group or type fields. Null or stale contexts
return `MrmFAILURE` instead of being dereferenced; valid contexts retain the
existing `MrmNUL_GROUP` and `MrmNUL_TYPE` results.
