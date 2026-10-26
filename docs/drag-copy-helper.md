# Harden the shared drag copy helper

The internal `_XmAllocAndCopy` helper is used by drag contexts and drop
transfers to duplicate caller-provided arrays.  It now checks that the byte
count fits Xt's `Cardinal` allocator, rejects a null source for non-empty
copies, and checks `XtMalloc` before copying.  Zero-length copies remain
valid and return a null pointer.

This prevents allocation-failure crashes while preserving the existing helper
interface and successful-copy behavior.  The focused source regression is
`tests/automated/check-drag-copy-helper.py`.
