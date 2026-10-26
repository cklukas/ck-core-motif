# MWM menu conversion results

The `_MWM_MENU` property is converted through Xmb before parsing.  MWM now
rejects an empty XTextProperty, then requires both a non-empty converted item count and a non-null first entry
before handing the result to the menu parser, while preserving the existing
cleanup path for empty or failed conversions.  Any returned text list is freed
even when validation rejects it.
The conversion output variables are initialized before the call so unusual
converter results remain safe to clean up.
Conversion failures also release any partial text list returned by Xmb.
