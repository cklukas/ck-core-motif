# Container drag-icon cache allocation

Container drag initiation keeps one cached drag-icon record per display. The
record is allocated before it is saved in the X context and initialized.

`DragStart` now checks this allocation and returns before touching the context
or record when memory is unavailable. Existing cached records and normal drag
behavior are unchanged.
