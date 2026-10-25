# WM_STATE read guards

`GetWMState` now requires the expected 32-bit format and exact item count
before returning a WM_STATE property. Invalid properties are freed and
reported as absent.
