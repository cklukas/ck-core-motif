---
nav_exclude: true
---

# ColorSelector initialization input

`Initialize` now validates its request/set widgets and creation argument list
before filtering child resources. A malformed initialization call returns
safely; normal Xt construction remains unchanged.

The focused source contract is checked by
`tests/automated/check-color-selector-initialize-input.py`.
