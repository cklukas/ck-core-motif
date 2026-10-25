# Mrm callback-valued argument bounds

Callback-valued argument descriptors now pass through the same legacy/current
record-size validation as creation callbacks before translation and fixup.  A
malformed callback offset is skipped through the existing argument conversion
failure path; valid callback arguments retain their registration and deferred
widget-reference behavior.

The source-level regression is
`tests/automated/check-mrm-arg-callback-record-bounds.py`.
