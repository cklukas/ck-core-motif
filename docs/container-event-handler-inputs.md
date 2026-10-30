---
nav_exclude: true
---
# Container event-handler input validation

Container enter and leave handlers now return safely for missing widgets,
missing crossing events, or a missing clipping parent before accessing event
or widget geometry during teardown and unusual dispatch.

The source contract is checked by
`tests/automated/check-container-event-handler-inputs.py`.
