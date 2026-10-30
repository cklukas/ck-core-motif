---
nav_exclude: true
---
# Icon-directory cache offset bounds

The icon-directory cache stores one 16-bit offset per filename plus a final
offset that marks the end of the last name.  The cache now allocates and
validates all `numFiles + 1` offsets before placing the name heap after them.
This prevents the terminal-offset write and subsequent heap placement from
running past the allocation while preserving the existing lookup format.

The combined name heap is still bounded by `USHRT_MAX`; directories whose
offsets cannot be represented use the existing uncached fallback rather than
silently truncating offsets.

The regression check is
`tests/automated/check-icon-cache-offset-allocation.py`.
