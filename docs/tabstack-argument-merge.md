# TabStack argument-list merging

TabStack filters application resources and merges them with the resources it
forwards to its internal TabBox. Both initialization and later resource updates
now reject a `Cardinal` count overflow and skip the operation cleanly when
`XtMergeArgLists` cannot allocate the merged list. Initialization also checks
the TabBox creation result before applying inherited appearance settings.

The focused source contract is covered by
`tests/automated/check-tabstack-argument-merge.py`.
