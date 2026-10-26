# Input-method registration input contract

`XmImRegister` now returns immediately when its widget is `NULL`, before
application-context lookup or parent-shell traversal.  This makes optional
registration during failed construction safe; valid registration behavior is
unchanged.
