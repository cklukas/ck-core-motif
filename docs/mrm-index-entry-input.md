# Mrm index-entry input validation

The high-level Mrm index entry routine and both root-record initializers now
validate their file, output-buffer, and index pointers before touching index
state. Valid index creation remains unchanged; malformed input returns
`MrmFAILURE` before allocating or dereferencing a record.

The source-level regression is
`tests/automated/check-mrm-index-entry-input.py`.
