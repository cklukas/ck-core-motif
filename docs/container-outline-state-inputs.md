---
nav_exclude: true
---
# Container outline-state input validation

Expand and collapse now validate their Container parent and constraint, and
skip malformed child nodes or missing child constraints before updating outline
visibility. Valid outline-state transitions remain unchanged.

The source contract is checked by
tests/automated/check-container-outline-state-inputs.py.
