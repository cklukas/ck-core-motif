---
nav_exclude: true
---
# Container traversal redirection input validation

Traversal redirection now returns the requested focus unchanged when the
Container has no clip-window parent, instead of passing a null parent to the
clip-widget class test. Normal traversal wrapping is unchanged.

The source contract is checked by
`tests/automated/check-container-redirect-traversal-inputs.py`.
