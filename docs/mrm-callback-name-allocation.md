---
nav_exclude: true
---
# Mrm deferred callback-name allocation

When a callback resource refers to a widget that has not been created yet,
Mrm keeps a copy of the referenced widget name for later resolution.  Callback
fixup now checks that copy operation and returns `MrmFAILURE` immediately when
memory cannot be allocated, before publishing an unresolved callback entry.
This keeps deferred-reference cleanup and retry paths from receiving a null
name while preserving the existing successful resolution behavior.

The source-level regression is
`tests/automated/check-mrm-callback-name-allocation.py`.
