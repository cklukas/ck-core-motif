---
nav_exclude: true
---
# Container button-down constraint guards

The Container button-down action now requires a valid child constraint before
reading selection state for a transfer or selection decision.  A hit-tested
widget can briefly lack private constraint storage during child lifecycle
changes; such input follows the existing non-transfer path instead of
dereferencing unavailable state.

The source contract is checked by
`tests/automated/check-container-button-down-constraint-guards.py`.
