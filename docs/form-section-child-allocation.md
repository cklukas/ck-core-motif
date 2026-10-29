---
nav_exclude: true
---
# FormSection child allocation

FormSection creates header, content, and footer children during initialization.
Geometry and resource updates now tolerate an individual child failing to
allocate, including the optional footer mapping path.

The contract is protected by
`tests/automated/check-form-section-child-failure-guards.py`.
