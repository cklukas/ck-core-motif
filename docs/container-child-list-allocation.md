---
nav_exclude: true
---
# Container child-list allocation

`XmContainerGetItemChildren` snapshots the linked children of a container into
an Xt-allocated array returned to the caller. The byte-count guard covers both
host `size_t` and Xt `Cardinal` limits and is followed by an allocation check;
a failed snapshot unlocks the application context and returns zero without
touching the missing array.
