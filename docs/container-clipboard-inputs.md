# Container clipboard command input contract

`XmContainerCut`, `XmContainerCopy`, and `XmContainerCopyLink` now return
`False` for a null widget before container casting, application-context setup,
or clipboard dispatch.  Valid container clipboard behavior is unchanged.
