# Tab-list insertion allocation

`XmTabListInsertTabs` now checks allocation of a new list and its first copied
tab in both insertion modes. It releases the partially created list, unlocks
the process state, and returns `NULL` on failure instead of dereferencing a
null tab or list.
