---
nav_exclude: true
---
# CutPaste conversion allocation guards

The legacy CutPaste converter now validates clipboard target counts before
computing the target-array size and checks both target-array and timestamp
allocations. Conversion reports failure cleanly when the request cannot be
represented or memory is unavailable.

The focused regression is
`tests/automated/check-cutpaste-conversion-allocation.py`.
