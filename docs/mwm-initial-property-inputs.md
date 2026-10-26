# MWM initial-property helper inputs

The WSM initial-property helpers in `clients/mwm/WmProperty.c` now handle a
missing `ClientData` record safely:

- `GetInitialPropertyList(NULL)` returns without querying X or modifying state.
- `DiscardInitialPropertyList(NULL)` returns without attempting a free.
- `HasProperty(NULL, atom)` returns `False`.

With a valid client record, the existing property-list capture, release, and
membership behavior is unchanged. The null cases are defensive contracts for
callers that are cleaning up or handling partially initialized client state.
