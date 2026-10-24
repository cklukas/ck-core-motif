# Mrm widget-instance allocation handling

Widget creation and set-values instance builders now check font-list, argument
array, callback-list, and scratch pointer-list setup.  Failure paths release
already-created scratch state before returning, preventing null dereferences
while preserving normal argument construction.

The source-level regression is
`tests/automated/check-mrm-widget-instance-allocation.py`.
