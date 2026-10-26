# MWM current workspace initialization

WSM client operations temporarily switch `currentWsc` while addressing a
client's per-workspace records. `GetClientInfo` now initializes that index to
workspace zero before any workspace-sensitive operation, preventing an
uninitialized index from selecting outside the workspace list.
