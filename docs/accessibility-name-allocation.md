---
nav_exclude: true
---
# Accessibility application-name allocation

`XmAccessibilityCreate` now treats failure to copy the application name as a
creation failure, releasing the partially initialized accessibility object
before returning. Successful accessibility registration and naming are
unchanged.

`tests/automated/check-accessibility-name-allocation.py` protects this
contract.
