---
nav_exclude: true
---
# MultiList argument-list merging

`XmMultiList` combines application-supplied arguments with small, internal
argument lists while it creates its title, scrollbars, search button, frame,
and list child.  Those combinations now reject a `Cardinal` count that would
wrap and treat a failed merged-list allocation as a missing optional child.
An empty title with no arguments remains valid: an empty argument list is
passed through as such.

The focused source contract is covered by
`tests/automated/check-multilist-argument-merge.py`.
