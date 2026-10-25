---
nav_exclude: true
---
# Mrm resource-descriptor bounds

Mrm widget-record resource descriptors now check index-string lengths before
adding them to the descriptor size and storing that size in the 16-bit
`MrmSize` field. Oversized indexes return `MrmFAILURE`; valid resource indexes
retain their existing record layout.

The source-level regression is `tests/automated/check-mrm-resource-bounds.py`.
