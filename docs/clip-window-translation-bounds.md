---
nav_exclude: true
---
# ClipWindow translation-buffer bounds

ClipWindow assembles generated virtual-key translations into a reusable
buffer.  The append helper now rejects a wrapped used-length calculation and
rejects results wider than Xt's `Cardinal` allocation size before growing the
buffer.  Existing failure behavior remains fail-closed: an allocation or
representability failure leaves the prior buffer valid.

`tests/automated/check-clip-window-translation-allocation.py` checks the
boundary guards alongside the existing allocation-failure checks.
