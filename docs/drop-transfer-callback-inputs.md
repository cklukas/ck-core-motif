---
nav_exclude: true
---

# Drop-transfer callback input validation

Drop-transfer completion, requestor-destruction, and drag-destruction
callbacks now reject a missing transfer object before touching transfer state.
Normal teardown and cancellation behavior is unchanged; malformed or late
callback delivery fails closed instead of dereferencing null client data.

The focused source-contract check is
`tests/automated/check-drop-transfer-drag-destroy.py`.
