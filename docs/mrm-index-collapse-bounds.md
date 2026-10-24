# Mrm index collapse bounds

Leaf and node index-collapse helpers now reject missing records, reversed or
out-of-range entry spans, impossible index counts, and malformed string
offsets before copying data.  They also bound temporary-heap growth, preserving
valid index splits while preventing malformed records from causing underflow,
out-of-record reads, or heap overwrites.

The source-level regression is
`tests/automated/check-mrm-index-collapse-bounds.py`.
