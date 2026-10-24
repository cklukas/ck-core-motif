# Mrm callback context-list initialization

Widget-instance creation and automatic-child setup now propagate failure when
their callback context list cannot be initialized.  Callback fixup no longer
receives a null list after an ignored allocation failure.

The source-level regression is
`tests/automated/check-mrm-callback-context-init.py`.
