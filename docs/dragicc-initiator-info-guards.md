---
nav_exclude: true
---
# Drag initiator-info property validation

`_XmReadInitiatorInfo` reads the byte-serialized
`_MOTIF_DRAG_INITIATOR_INFO` property.  It now requires the expected atom,
8-bit format, a complete X property, and enough bytes for the fixed header
before interpreting the record.
