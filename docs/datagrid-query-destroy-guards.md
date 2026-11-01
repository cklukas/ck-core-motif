---
nav_exclude: true
---

# DataGrid query teardown guards

`XmDataGrid` mutators already decline work during Xt destruction. Its public
query APIs now do the same before reading model or edit storage: counts and the
top row return zero, cell/edit queries return false or `NULL`, and editing state
returns false. Normal live-widget results are unchanged.

This lets a destroy callback safely use the same defensive query pattern as
other Motif composite widgets without observing partially released data.
