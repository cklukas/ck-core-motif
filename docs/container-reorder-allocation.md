---
nav_exclude: true
---
# Container reorder allocation

`XmContainerReorder` snapshots position indexes into a temporary integer array
before resequencing sibling items. The public entry point now rejects a null
widget list and checks both host arithmetic and Xt's `Cardinal` width before
sorting or dereferencing it. Failure releases the application lock and leaves
the current order unchanged.
