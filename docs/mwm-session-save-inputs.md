# MWM session persistence input boundaries

MWM session save and restore helpers can run while session shutdown or client
teardown has left only a partial manager state.  The public callback and
database-entry paths now fail closed when their top-level shell, screen array,
client record, client screen, or workspace-list state is absent.

A valid client still serializes the same geometry, state, identity, and
workspace/icon data.  If a workspace list is absent, the valid client record
is retained but its unavailable workspace-specific icon entries are omitted.
`FindClientDBMatch` now initializes its optional workspace-name output to null,
and a failed database close is reported as a failed checkpoint instead of
loading a potentially incomplete file.

Focused check:

```
python3 tests/automated/check-mwm-session-save-inputs.py
```
