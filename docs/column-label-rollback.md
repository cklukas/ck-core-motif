# Column label rollback

`XmColumn` constraint updates now copy a replacement label before releasing
the current widget-owned string.  Failed copies keep the current label and do
not update the child with a NULL value.

The focused regression is `tests/automated/check-column-label-rollback.py`.
