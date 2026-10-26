---
nav_exclude: true
---
# Clipboard transfer status scope

The clipboard-manager ownership branch now uses a distinct
`selection_status` variable. This avoids shadowing the enclosing transfer
status and keeps the ownership result separate from the Motif clipboard
operation result.

The focused regression is
`tests/automated/check-transfer-status-scope.py`.
