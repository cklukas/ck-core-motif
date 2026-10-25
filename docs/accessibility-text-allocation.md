---
nav_exclude: true
---

# Accessibility text extraction allocation guard

Text extraction now checks the temporary locale buffer against the Xt
`Cardinal` limit and returns cleanly when allocation fails, before invoking
the text or text-field substring APIs.

Valid text extraction is unchanged.  The static regression check is
`tests/automated/check-accessibility-text-allocation.py`.
