# Extended text-segment separator bounds

The text-property encoder's extended-segment decoder now bounds its STX
separator scan by the segment length.  A malformed segment without a
separator is rejected before the old unbounded loop could read past the
declared buffer.

The focused source contract is checked by
`tests/automated/check-resencod-segment-bounds.py`.
