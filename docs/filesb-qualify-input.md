---
nav_exclude: true
---

# FileSelectionBox search qualification input

`QualifySearchDataProc` now rejects missing input or output callback records
before accessing their fields. It also leaves the corresponding length at
zero when compound-string generation fails, rather than measuring a null
`XmString`. The documented nullable search fields and normal qualification
behavior remain unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-qualify-input.py`.
