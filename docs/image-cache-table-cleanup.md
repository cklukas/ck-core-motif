# Scaled image table cleanup

Scaled image rendering builds four coordinate tables for source pixels.  The
cleanup path now releases the x, y, width, and height tables exactly once.  A
previous typo released the y table twice and leaked the x table, so allocation
failure and normal rendering could either double-free memory or retain the
x-coordinate table.

The focused regression is
`tests/automated/check-image-cache-table-cleanup.py`.  It checks the cleanup
block in `_XmPutScaledImage` for one release of each table and rejects the
old duplicate-y pattern.
