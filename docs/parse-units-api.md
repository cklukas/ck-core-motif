---
nav_exclude: true
---
# XmeParseUnits input validation

`XmeParseUnits` now rejects a null unit specification or output pointer with
`XmPARSE_ERROR` before inspecting the string.  Callers can therefore pass
malformed resource data through the normal parse-failure path safely.
