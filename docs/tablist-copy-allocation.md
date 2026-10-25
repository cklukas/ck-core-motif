---
nav_exclude: true
---
# Tab-list copy allocation

`XmTabListCopy` now checks its result-list allocation, rejects an empty source
range, and checks the first copied tab before initializing the list. Failures
release the partial list and unlock process state instead of dereferencing null
tab/list pointers.
