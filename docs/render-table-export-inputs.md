# Render-table export inputs

`XmRenderTableCvtToProp` now rejects a null widget, render table, or output
pointer before entering the application context or serializing entries.  This
matches the defensive behavior of the import converter and keeps malformed
converter calls from dereferencing invalid state.
