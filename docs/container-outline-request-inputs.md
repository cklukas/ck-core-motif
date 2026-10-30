---
nav_exclude: true
---
# Container outline geometry request validation

Outline-detail geometry requests now return safely when the Container widget or
requested geometry record is missing. Valid layout negotiation and redraw
behavior are unchanged.

The source contract is checked by
`tests/automated/check-container-outline-request-inputs.py`.
