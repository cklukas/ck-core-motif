# Container reorder input contract

`XmContainerReorder` now returns before context lookup when its widget or child
list is absent, when fewer than two children are supplied, or when any listed
child is null. Valid reorder operations are unchanged, while malformed caller
input no longer reaches constraint lookup or sorting.
