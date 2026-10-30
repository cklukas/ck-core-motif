---
nav_exclude: true
---
# Container selection dispatch input validation

Selection start now returns safely when its widget or event is absent and no
longer dereferences a missing child constraint while handling a selected item.
Valid selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-start-select-inputs.py`.
