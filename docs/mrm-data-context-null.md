---
nav_exclude: true
---
# Mrm data-entry context null guard

`Idb__DB_GetDataEntry` now checks for a null resource context before invoking
the validation macro. Public indexed-resource and RID retrieval calls with a
null context return the existing bad-context error instead of dereferencing a
null pointer. Valid contexts follow the historical path unchanged.
