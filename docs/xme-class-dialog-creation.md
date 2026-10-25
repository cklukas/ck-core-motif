# Xme class-dialog creation

`XmeCreateClassDialog` now treats child-widget creation as part of the dialog
transaction.  If the requested child cannot be created, the temporary dialog
shell is destroyed and the helper returns `NULL` before registering callbacks
on a null child.  Successful shell/child creation and callback behavior are
unchanged.
