# SpinBox value-list initialization rollback

SpinBox now rolls back a partially copied string-child value list when an
`XmStringCopy` fails during constraint initialization.  Earlier copies and the
array are released, and the child publishes an empty list instead of exposing
NULL entries to position and text-update logic.

The focused regression is
`tests/automated/check-spinbox-value-init-rollback.py`.
