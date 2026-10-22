# Native accessibility

`<Xm/Accessible.h>` provides a native semantic-provider layer. The optional
AT-SPI D-Bus transport is built with `--enable-atspi`; no ATK or GTK dependency
is introduced. Normal widget operation does not depend on a running bus or
registry.

## Bridge and shell ownership

Create one application bridge with `XmAccessibilityCreate(root, name)`.
Additional top-level shells in the same Xt application context attach through
`XmAccessibilityAttachShell`. Closing one attached shell does not destroy the
application bridge. Destroy the bridge with `XmAccessibilityDestroy` before
its root widgets and application context are destroyed.

All calls, provider queries, callbacks and event dispatch run on the owning Xt
UI thread. The transport does not make background calls into Xt. Do not free
provider closures while their nodes remain registered.

## Semantic nodes

Native widgets receive semantic identities through
`XmAccessibilityWidget`. Add application-supplied semantic children below a
widget or another semantic node using `XmAccessibilityAdd`. IDs are not reused
during a bridge lifetime. Remove nodes with `XmAccessibilityRemove` before
releasing their data.

`XmAccessibleProvider` supplies query, bounded text range, action, action-name,
focus and table-cell callbacks. Query results contain role/state, names,
optional screen extents, scalar text length/caret, action count and optional
table dimensions. Screen extents are screen-pixel coordinates.

Owned strings returned in `XmAccessibleInfo` use `XtMalloc`/`XtNewString`; the
query caller releases them with `XmAccessibleInfoClear`. Text callbacks return
owned UTF-8 for an end-exclusive scalar range, rather than copying an entire
large text document for each unrelated property query. `XmAccessibilityText`
returns `NULL` for invalid, stale or unavailable ranges. Callers release a
successful range result with `XtFree`.

Application actions act on their semantic owner. A canvas can expose semantic
children without pretending that pixels themselves are accessible text or
without instantiating thousands of hidden widgets. Register only identities
that remain valid for the displayed state.

## Events and optional services

`XmAccessibilityRefresh` refreshes native structure;
`XmAccessibilityChanged` publishes property/state changes for an identity.
Text and caret changes have bounded native notifications. A multi-shell
application should test node destruction, focus changes and event ordering
across shell closure/recreation.

`XmAccessibilityConnect` returns an explicit status. A transport-disabled build
reports `NOT_BUILT`; unavailable services, connecting, connected and disconnected
states are distinct. `CONNECTED` requires the registry Embed handshake, not
merely opening a session bus connection. Applications can inspect status through
`XmAccessibilityGetStatus` and the exported name through
`XmAccessibilityBusName`.

Test with services absent as well as with a real registry and assistive client.
A successful fixture handshake alone does not establish complete screen-reader
usability. See [current validation boundaries](status.md).
