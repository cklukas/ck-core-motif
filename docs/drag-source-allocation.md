---
nav_exclude: true
---
# Drag-source setup allocation

`XmeDragSource` copies caller arguments and obtains export targets before
starting a drag. It now validates argument-count arithmetic, rejects a
missing argument array when entries were requested, checks the copied-array
allocation, and initializes the target pointer before conversion.

A failed export-target conversion therefore follows the existing cleanup path
without freeing an uninitialized pointer. Successful drag setup and argument
ordering remain unchanged.
