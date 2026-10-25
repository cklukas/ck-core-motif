---
nav_exclude: true
---
# Protocol-manager removal index safety

When a vendor shell was destroyed with more than one protocol manager,
`RemoveProtocolMgr` reused one loop variable for both the protocol entries and
the manager list.  The manager compaction therefore started at the number of
protocols instead of the removed manager's index, leaving a freed manager in
the live list and crashing during destruction.

The cleanup now keeps independent manager and protocol indices.  This retains
the public protocol API and makes multi-manager shell destruction safe.  The
focused source check is
`tests/automated/check-protocol-manager-removal-index.py`; the installed C++
consumer lifecycle fixture exercises the destruction path.
