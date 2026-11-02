---
nav_exclude: true
---

# MWM front-panel registration

The embedded-client, push/recall-client, and icon-box registration callbacks
resolve their panel widget through one checked managed-screen lookup. Calls made
before a screen is managed, after panel teardown, with a foreign widget, or
without a screen table do nothing. Embedded and push/recall registrations also
reject negative counts and a nonzero count with no record array. A valid
zero-count registration still clears the previously registered list.

Push/recall registration resets both components of each accepted timeout so no
stale partial deadline survives replacement registration.

Matching a managed client to an embedded or push/recall record validates the
client, screen, record table, output pointer, and resource names before
comparing strings. An incomplete record is ignored rather than making later
panel scanning dereference or compare a null pointer.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-panel-registration.py
```
