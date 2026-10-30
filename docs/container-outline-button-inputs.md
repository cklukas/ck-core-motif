---
nav_exclude: true
---
# Container outline-button input validation

Outline-button creation now validates the source child, parent, and
constraint, and unwinds cleanly if gadget creation or its constraint record
fails. Valid outline-button creation is unchanged.

The source contract is checked by
tests/automated/check-container-outline-button-inputs.py.
