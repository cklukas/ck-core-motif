---
nav_exclude: true
---
# Legacy translation-buffer bounds

The compatibility `_XmGetRealXlations` path now rejects a used-length value at
`SIZE_MAX` before evaluating the append subtraction.  This matches the modern
ClipWindow translation builder and prevents boundary wraparound while
retaining the existing fail-closed allocation behavior.

`tests/automated/check-real-xlations-growth.py` verifies both translation
builders' width and growth guards.
