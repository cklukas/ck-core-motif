# Mwm workspace property-name bounds

Mwm derives `_DT_WORKSPACE_INFO_*` atom names from the workspace resource
name. The constructor now performs the length calculation in `size_t`, checks
both arithmetic overflow and the `XtMalloc` `Cardinal` limit, and reports an
unrepresentable name to its callers before allocating. The two property
callers skip the X request safely when the name cannot be represented.

`tests/automated/check-workspace-property-name-bounds.py` guards this checked
allocation contract.
