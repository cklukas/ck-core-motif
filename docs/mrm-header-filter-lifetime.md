# Mrm header filter buffer lifetime

`Idb__HDR_MatchFilter` now keeps the header buffer live while validating and
reading the data-entry header.  It computes the filter result first and only
then decommits the buffer, preventing a use-after-decommit while also checking
record and entry bounds.

The source-level regression is
`tests/automated/check-mrm-header-filter-lifetime.py`.
