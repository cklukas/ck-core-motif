---
nav_exclude: true
---
# Container view and detail input validation

View-type updates now return safely for missing children or parents, icon-width
lookup returns zero for a missing child, and default-detail counting skips
malformed nodes or an absent Container widget. Valid trait and detail counting
behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-view-type-inputs.py`.
