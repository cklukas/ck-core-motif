# UIL listing title-one bounds

The first listing title combines the compiler name, version, and timestamp in
a fixed 132-byte buffer.  These values are normally short, but build variants
or localized time strings can make the old `sprintf` write past the buffer.

The title now uses bounded `snprintf`; normal title output is unchanged and
oversized metadata is truncated at the listing boundary.

`tests/automated/check-uil-title1-bounds.py` protects this contract.
