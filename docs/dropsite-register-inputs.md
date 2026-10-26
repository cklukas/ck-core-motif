# Drop-site registration input contract

`XmDropSiteRegister` requires a widget to identify the display and drop-site
manager.  It now returns without changing state when passed `NULL`, before
application-context or display lookup.  This makes optional registration
paths safe when widget construction has failed or teardown has already begun.

Registration behavior for valid widgets is unchanged.
