# AddEntryToList guards

Window-list linking can be reached while a workspace is being detached.
`AddEntryToList` now returns when the workspace, its screen, or the entry is
missing, preventing partial list writes and null dereferences.  Valid list
ordering behavior is unchanged.
