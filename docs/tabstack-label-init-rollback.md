# TabStack tear-off label initialization rollback

When tear-off tabs are enabled, `XmTabStack` now checks the initial
`XmStringCopy` for `XmNtearOffLabelString`.  If the copy cannot be allocated,
the widget installs an empty compound string instead of propagating a NULL
label into the tear-off button and later geometry or destruction paths.

The focused regression is
`tests/automated/check-tabstack-label-init-rollback.py`.
