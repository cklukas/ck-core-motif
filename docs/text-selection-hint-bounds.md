# Text selection-hint bounds

Primary and secondary selection-start hints now saturate button-event
coordinates before storing them in the 16-bit hint fields used by drag and
multi-click selection. This keeps later distance checks and hit-tests
consistent for translated or unusually large coordinates.

The focused source contract is checked by
`tests/automated/check-text-selection-hint-bounds.py`.
