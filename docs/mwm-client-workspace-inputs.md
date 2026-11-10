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
The public session-match entry point and both XSMP and proxy match paths also
reject a missing client; database lookup requires a client ID, format string,
and initialized resource database.  This makes partial session restore state a
safe non-match rather than a window-manager failure.

Regression: `tests/automated/check-mwm-client-workspace-inputs.py`.
