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
Native Text/TextField value callbacks publish exact `TextChanged:delete` and
`TextChanged:insert` deltas, including programmatic changes to read-only text.
Offsets and lengths count Unicode scalars; event payloads are owned UTF-8 copies
of the removed/inserted range. Replacements delete before inserting at the same
offset. A common scalar prefix/suffix is preserved; the remaining contiguous
range describes the change without inventing keystrokes. Rejected edits,
refreshes, unchanged content and caret-only movement emit no text-edit event.
Caret notifications describe actual caret changes.

The optional transport retains one current text baseline per live text node,
proportional to current exposed content, and reads providers in ranges of at
most 1024 scalars. Initial connection/new-node observation captures a baseline
without announcing existing text as newly inserted. Removal frees the baseline;
no historical text or IDs accumulate. Failed, malformed, unavailable, excessive
allocation-size or reentrant observations invalidate that baseline; the next
successful observation establishes a new baseline without claiming an exact
missed edit. D-Bus events are constructed as a complete ordered batch before queueing.
Construction failures abandon open containers and discard the batch; a failed
queue operation invalidates the baseline, since a client might have observed
only part of a replacement. These failures cannot prove delivery of an edit.
Provider callbacks must supply valid, stable UTF-8 and call
`XmAccessibilityChanged` after each accepted semantic text change.

Providers representing secrets must use `XmACCESSIBLE_PASSWORD_TEXT`; the bridge
neither snapshots nor returns their text and emits no content-bearing deltas.
Native Text/TextField has no password resource: applications that mask secrets
using custom modify-verify logic must expose a password semantic provider rather
than exposing its underlying native storage. A multi-shell
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

## Native menus

Native MenuBar, PulldownMenu and PopupMenu containers expose menu roles.
PushButton and ToggleButton entries, including gadgets, expose menu-item,
check-menu-item or radio-menu-item roles; passive labels retain label semantics.
Cascade buttons expose menu-item actions and native has-popup/expandable/expanded
states. A shared pane expands only its current posting cascade. Expanded follows
actual pane visibility, rather than its managed state.

Menu actions use the native class activation path, which owns posting, callback
delivery, focus and grab cleanup. Logical focus is read from the actual input
shell, including a posted MenuShell; its owning WM frame retains activation.
This prevents menu leaves from losing their focused state merely because the
menu uses an override shell.

`tests/accessibility/menu.c` covers native roles, widget/gadget entries, shared
posting, leaf focus, action delivery, cleanup and invalid/insensitive actions.
These additions pass the strict native ARM64 installed feature suite. Actual keyboard/Orca journeys and native List selection remain separate
qualification gates.


## Native keyboard listeners

When the optional transport is connected, registered native widgets forward
actual KeyPress/KeyRelease input to the registry's `NotifyListenersSync` method.
A raw Xt handler at the head of the widget's handler list runs before native
translations/actions, without selecting additional X input or replacing the
application's event dispatcher. A listener's successful `true` reply consumes
the event; otherwise native processing continues. Unavailable services, invalid
replies, send failures and timeouts fail open.

The transport waits at most 500 ms for a reply, while servicing accessibility
D-Bus requests on the owning UI thread. It does not dispatch another X event or
run a nested Xt main loop. A nested native key dispatched by application code
during a provider/action callback continues normally without starting a second
listener wait. Callback execution itself remains subject to the provider's UI
thread contract. Destruction requested by a callback is deferred until active
dispatch finishes; no late listener reply retroactively consumes a delivered key.
Hooks are removed when their node/bridge is destroyed, including attached shells.

Key identity, hardware code, modifiers and X timestamp come from the native
event. Printable Latin-1 and Unicode keysyms use UTF-8 labels; other keysyms use
their X11 names. Lookup does not modify the widget's input-method or compose
context. The notification describes physical input, not committed text: the
ordinary text provider reports accepted edits independently. Toolkit listeners
are distinct from global X11 or compositor monitoring. For example, AT-SPI's
legacy device backend registers non-global listeners and requires toolkit
forwarding even when the application's X input is delivered through Xwayland.

### Registry device-event wire types

The registry-input and listener-output wire contracts differ. The native bridge
sends `(uiiiisb)` to `NotifyListenersSync`: type unsigned32, keysym signed32,
hardware signed32, modifiers signed32, timestamp signed32, label string and
printable boolean. Native X11 hardware codes and the 16-bit modifier mask fit
exactly in those signed input fields. The registry's actual parser accepts them;
its advertised `(uiuuisb)` signature is rejected for this method.

The registry forwards `(uiuuisb)` to listener `NotifyEvent`, with unsigned32
hardware/modifier fields. The listener test preserves that output signature and
checks every field's actual D-Bus type rather than changing its expectations to
match the bridge's input. This asymmetry follows the actual parser and marshaller
in [AT-SPI 2.56.2 registryd/marshal-dbus.c](https://github.com/GNOME/at-spi2-core/blob/2.56.2/registryd/marshal-dbus.c),
SHA-256 `c92dbc7750295ec06733c1ce56cfdce667d457575c412a8bec0cfbf4daaa97c5`.
No alternative signature negotiation or application-specific forwarding is used.

The signed registry-input correction passes the focused installed-prefix ARM64
listener fixture and the complete 16-fixture native suite, including before-edit
queries, consumption, attached shells,
nested input, timeout/disconnect and teardown. An earlier private registry capture
rejected unsigned input with `InvalidArgs` before the native edit continued.
Actual screen-reader input echo, input methods and sanitizer qualification of the
new key path remain separate gates.
