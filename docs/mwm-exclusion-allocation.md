---
nav_exclude: true
---
# MWM menu-exclusion allocation

Menu resource parsing stores exclusion strings for later command matching.
Previously, allocation failure left the parser writing through a null
`MenuExclusion` record or retained a partially initialized record.  The
parser now ignores an exclusion when its record or copied command string
cannot be allocated, and frees the record before returning.  Valid exclusions
keep the same ordering and matching behavior.

The client-command parser also initializes the temporary exclusion-text
pointer and passes it to `StoreExclusion` only after a `~` modifier has
captured a value.  This makes the ownership boundary explicit and prevents a
malformed future parser path from passing an indeterminate pointer.

`tests/automated/check-mwm-exclusion-allocation.py` protects this contract.
