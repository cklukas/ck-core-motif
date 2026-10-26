# Drop-site retrieval input contract

`XmDropSiteRetrieve` synchronizes and returns drop-site information for an
enclosing widget.  A null enclosing widget cannot identify a display manager;
the function now returns immediately before application-context or display
lookup.  Retrieval for valid widgets is unchanged.
