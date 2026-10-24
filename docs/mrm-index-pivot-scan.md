# Mrm index pivot scan bounds

Leaf and node index splits now bound pivot-name scans by both the maximum index
name length and the bytes remaining in the serialized record.  A corrupt offset
near the record boundary cannot make `strnlen` read beyond the record before
the existing malformed-pivot rejection runs.

The source-level regression is `tests/automated/check-mrm-index-pivot-scan.py`.
