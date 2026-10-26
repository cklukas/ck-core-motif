# Container relayout input contract

`XmContainerRelayout` now returns immediately for a null widget before
application-context lookup, realization checks, or container casting.  Valid
container relayout behavior is unchanged.
