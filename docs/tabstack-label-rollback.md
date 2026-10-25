# TabStack label rollback

`XmTabStack` now copies changed tab and tear-off labels before releasing the
current `XmString`. Failed copies preserve the existing label, while NULL
continues to mean an explicit clear.

The focused source regression is
`tests/automated/check-tabstack-label-rollback.py`.
