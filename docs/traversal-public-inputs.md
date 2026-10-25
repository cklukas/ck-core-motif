---
nav_exclude: true
---

# Traversal public-input validation

Traversal predicates, focus helpers, visibility, and tab-group queries now
return safe defaults when passed a missing widget. Valid widgets continue to
use the existing application-context and focus-data behavior.

The focused regression check is
`tests/automated/check-traversal-public-inputs.py`.
