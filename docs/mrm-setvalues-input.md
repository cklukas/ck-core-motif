---
nav_exclude: true
---
# Mrm setter input validation

`UrmFetchSetValues` interprets each `Arg.value` as the name of a literal in a
UID hierarchy. The setter now rejects a nonzero argument count with a null
argument array, and rejects null literal-index values before measuring or
copying them. Existing valid argument lists follow the same fetch and
conversion path; malformed input returns `MrmFAILURE` instead of faulting in
`strlen` or `strcpy`.

The source-level regression is `tests/automated/check-mrm-setvalues-input.py`.
