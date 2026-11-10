---
nav_exclude: true
---

# MWM client workspace input validation

MWM's session workspace-name aggregation now validates its `ClientData` input
before reading the associated screen pointer.  A null or incomplete client
record produces no workspace list instead of dereferencing incomplete session
state.

The exported icon-position loader, proxy-property reader, and XSMP resource
lookup apply the same boundary: incomplete client, screen, workspace-list,
session-ID, or format state is ignored safely instead of being dereferenced.

Regression: `tests/automated/check-mwm-client-workspace-inputs.py`.
