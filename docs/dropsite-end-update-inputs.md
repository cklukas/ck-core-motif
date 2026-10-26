# Drop-site update completion input contract

`XmDropSiteEndUpdate` completes a grouped update for a reference widget and
its display's drop-site manager.  A null reference cannot identify those
resources, so the function now returns before application-context or display
lookup.  Valid-widget update completion is unchanged.
