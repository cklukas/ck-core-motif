# Font selector metadata allocation

Font selector initialization now checks its `FontInfo` and family-table
allocations. Family-table growth is transactional, and initialization returns
cleanly when metadata cannot be built instead of dereferencing failed storage.
The existing widget ABI and normal font discovery behavior are unchanged.

The source-level regression check is
`tests/automated/check-fonts-metadata-allocation.py`.
