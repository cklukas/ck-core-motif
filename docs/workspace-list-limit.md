---
nav_exclude: true
---
# Mwm workspace-list limit

Mwm has always limited the resulting workspace set to `MAX_WORKSPACE_COUNT`
(64). Workspace-list parsing now applies that same limit while collecting
resource names, so extra entries are ignored before the temporary name-pointer
array grows. Existing configurations with 64 or fewer workspaces keep the same
names and ordering, while oversized resource values no longer cause needless
allocation and parsing work.

`tests/automated/check-workspace-list-limit.py` guards the bounded parser loop.
