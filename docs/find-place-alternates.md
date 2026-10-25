# Alternate icon-place bounds

The alternate-position search in `FindIconPlace` now validates each converted
place before reading the placement list.  Invalid negative or oversized
coordinates are skipped rather than becoming an out-of-bounds access.

`tests/automated/check-find-place-alternates.py` protects the lookup contract.
