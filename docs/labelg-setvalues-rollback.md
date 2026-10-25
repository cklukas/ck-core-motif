# LabelGadget SetValues rollback

LabelGadget now retains its current label or accelerator text when copying a
changed valid `XmString` fails.  The previous value is no longer freed while
the replacement is unavailable, preventing labels and menu accelerators from
disappearing during transient allocation pressure.

The focused regression is
`tests/automated/check-labelg-setvalues-rollback.py`.
