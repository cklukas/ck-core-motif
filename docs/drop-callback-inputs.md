---
nav_exclude: true
---
# DnD callback teardown inputs

Drop callbacks can run while a drag is being cancelled or its owner is being
destroyed.  `DropDestinationHandler` now validates its widget and callback
structure before obtaining the display, and `DeleteDropCBStruct` validates
the transfer ID and callback structure before freeing destination data.
Malformed teardown callbacks therefore return safely without null
dereferences or invalid frees.

The focused regression is `tests/automated/check-drop-callback-inputs.py`.
