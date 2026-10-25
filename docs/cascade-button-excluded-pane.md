---
nav_exclude: true
---
# CascadeButton excluded-pane initialization

Cascade buttons record the submenu being posted in shared excluded-parent-pane
storage used by tear-off restoration. The storage is normally initialized on
the first cascade, but a failed allocation or a malformed zero-capacity state
must not be dereferenced.

Initialization now uses a temporary replacement pointer and commits the pointer
and four-entry capacity only after Xt allocation succeeds. The cascade returns
without changing menu state when storage cannot be obtained.
