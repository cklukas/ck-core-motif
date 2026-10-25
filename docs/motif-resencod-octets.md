# Compound-text encoding buffer allocation

When a compound-text segment combines a registered encoding prefix with the
current item, the temporary octet buffer is now checked before copying. An
allocation failure returns a null conversion result instead of dereferencing
invalid storage.

The regression check is
`tests/automated/check-resencod-octet-allocation.py`.
