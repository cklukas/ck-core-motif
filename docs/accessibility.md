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

Lookup tables grow with live objects, not historical IDs. Children retain their
insertion order and exact parent index. Subtree removal invalidates every ID
immediately and tears down descendants iteratively. Provider callbacks may
remove nodes; internal dispatch guards retain retired storage until the active
call finishes. Destroying a bridge from a callback defers final release to the
owning Xt event loop. Provider data must still obey the caller's lifetime
contract; deferred internal storage is not ownership of application payloads.

`XmAccessibleProvider` supplies query, bounded text range, action, action-name,
focus, table-cell and value-setting callbacks. Query results contain role/state, names,
optional screen extents, scalar text length/caret, action count and optional
table dimensions. Screen extents are screen-pixel coordinates.

Owned strings returned in `XmAccessibleInfo` use `XtMalloc`/`XtNewString`; the
query caller releases them with `XmAccessibleInfoClear`. Text callbacks return
owned UTF-8 for an end-exclusive scalar range, rather than copying an entire
large text document for each unrelated property query. `XmAccessibilityText`
returns `NULL` for invalid, stale or unavailable ranges. Callers release a
successful range result with `XtFree`.

Zero-initialize the provider record. Its optional `visual_line` callback uses
scalar offsets, an exclusive end and directions -1/0/+1 for previous/current/
next actual layout lines. Native wrapped Text uses its authoritative line
table; TextField has one line. A hard newline belongs to its preceding line,
and a trailing newline leaves a final empty line. Unknown layout returns
unavailable rather than inventing visual wrapping from logical paragraphs.

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

The development transport supplies the actual live Cache tree and structural
add/remove references. Native frame ACTIVE follows actual input-focus ownership,
including transient and override popup ownership, rather than treating every
mapped shell as active. Initial state is captured after the registry handshake;
subsequent checked/focused/expanded/name/value notifications describe deltas.

`XmAccessibilityConnect` returns an explicit status. A transport-disabled build
reports `NOT_BUILT`; unavailable services, connecting, connected and disconnected
states are distinct. `CONNECTED` requires the registry Embed handshake, not
merely opening a session bus connection. Applications can inspect status through
`XmAccessibilityGetStatus` and the exported name through
`XmAccessibilityBusName`.

Test with services absent as well as with a real registry and assistive client.
A successful fixture handshake alone does not establish complete screen-reader
usability. See [current validation boundaries](status.md).

## Numeric values and native scroll bars

Native `XmScrollBar` nodes expose the AT-SPI Value interface, orientation,
focus, keyboard actions and value-change events. The reachable upper value is
`maximum - sliderSize`, matching the native control. Read-only or insensitive
controls reject writes. Integral native controls reject fractional, nonfinite
and out-of-range requests; successful writes use the normal widget callback
path. Reversed processing direction is preserved for native keyboard actions.

Custom providers describe an available range and current value in
`XmAccessibleInfo`, and implement `set_value` when editable. A provider may
accept continuous values; the bridge does not impose the native ScrollBar's
integral restriction on an unrelated control. Providers own domain validation
and emit changes after a successful update. Rebuild all providers for ABI6:
the public info and provider record layouts have changed.

Focused native observer tests cover introspection, properties, writes,
actions, callbacks, focus, orientation, dynamic ranges, service absence and
stale-node/destruction handling. Full installed-cohort verification and actual
screen-reader qualification are separate gates.
