# Drop-site unregistration input contract

`XmDropSiteUnregister` removes a widget's drop-site registration and updates
the display's drop-site manager.  A null widget cannot identify either
resource, so the function now returns immediately before application-context
or display lookup.  This makes teardown and conditional cleanup safe without
changing valid-widget behavior.
