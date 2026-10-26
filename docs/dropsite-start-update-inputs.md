# Drop-site update startup input contract

`XmDropSiteStartUpdate` begins a grouped update for a reference widget and
its display's drop-site manager.  A null reference has no display or manager;
the function now returns immediately before application-context and display
lookup.  Existing behavior for valid reference widgets is unchanged.
