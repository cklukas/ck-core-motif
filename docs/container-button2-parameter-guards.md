---
nav_exclude: true
---
# Container button-two parameter guards

The button-two down and motion actions now reject a missing action-parameter
count before validating or using their parameter list.  This keeps malformed
or partially initialized action dispatches on the existing warning/no-op path;
normal Xt dispatch is unchanged.

The source contract is checked by
`tests/automated/check-container-button2-parameter-guards.py`.
