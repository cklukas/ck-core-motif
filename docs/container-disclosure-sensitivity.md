---
nav_exclude: true
---
# Container disclosure sensitivity

Container disclosure buttons are synthetic siblings of the real outline row.
`OutlineButtonAction` now checks the row handle and its effective sensitivity
before reading constraints, changing `outline_state`, or delivering the
outline callback.  A disabled row therefore cannot be expanded or collapsed by
an activated proxy gadget or a keyboard action.

The guard is deliberately before parent/constraint access and leaves enabled
rows and all callback ordering unchanged.  The focused source regression is
`tests/automated/check-container-disclosure-sensitivity.py`.
