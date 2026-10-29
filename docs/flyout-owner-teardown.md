---
nav_exclude: true
---
# Flyout owner teardown

Flyout may receive an owner-destroy callback while its popup or label is also
being torn down. Owner callbacks, popup destruction, and label resource updates
now check widget validity and `being_destroyed` state before touching those
objects.

The contract is protected by
`tests/automated/check-flyout-owner-teardown.py`.
