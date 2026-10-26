# Text selection-transfer point bounds

The selection-transfer path stores the initiating X event location in an
`XPoint`, whose coordinates are 16-bit fields even though X event coordinates
are exposed as `int`. The conversion now saturates at the representable
`Position` bounds instead of wrapping, preserving a stable paste/drop location
for translated or unusually large coordinates.

The focused source contract is checked by
`tests/automated/check-text-transfer-point-bounds.py`.
