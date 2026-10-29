---
nav_exclude: true
---
# List selection and geometry API inputs

The public List helpers `XmListGetSelectedPos`, `XmListSetHorizPos`,
`XmListYToPos`, `XmListUpdateSelectedList`, and `XmListPosSelected` now reject
non-List widget handles before context lookup or private-state access.  Their
existing null, range, output, and callback behavior for valid List widgets is
unchanged.

The focused source regression is
`tests/automated/check-list-api-inputs.py`.
