# RowColumn accelerator expansion

RowColumn expands virtual menu-bar and menu key bindings into real Xt
translations. The expansion now grows its temporary storage as needed and
checks size arithmetic instead of writing into a fixed 1000-byte stack buffer.
The resulting accelerator string and public menu behavior remain unchanged.
