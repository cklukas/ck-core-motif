# Drop-site update input contract

`XmDropSiteUpdate` applies drop-site arguments to an enclosing widget and its
display manager.  A null enclosing widget cannot identify those resources;
the function now returns immediately before application-context or display
lookup.  Valid-widget update behavior is unchanged.
