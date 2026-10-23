# Optional native accessibility foundation

`modality.c` is an installed-library mutation-admission fixture. Run it only on
an isolated X server with `--isolated-display`. Its native button/list/value and
custom action/focus/value/Selection callbacks check primary and nested primary
scope, independent windows, full modality, direct Xt exclusive grabs and
popdown/destruction restoration. Provider reads that open a modal, remove a
semantic target or destroy its native owner must prevent the subsequent mutation
callback. Application-root-only semantics remain readable and reject mutations.
The fixture does not connect an AT-SPI service or establish screen-reader
acceptance. `selection_reentrancy.c` separately tests stable selection snapshots
with its mutable semantic object explicitly anchored to its owning shell.
The current installed feature runner includes `accessibility-modality` in its
21-fixture default suite and accepts `--only accessibility-modality` for focused
normal or sanitizer runs. Earlier 20-fixture acceptance below predates this
addition; it does not validate the new admission contract.

Configure `--enable-atspi` to build the libdbus transport. Default builds retain
native/custom semantic providers and report `XmACCESSIBILITY_NOT_BUILT`. The
transport adds libdbus only; a session bus, AT-SPI bus/registry and assistive
technology are optional external services. They are not GTK/Qt or desktop-shell
requirements. Start the bridge on the Xt thread after setting the process locale.
Destroy it before its widgets. Refresh when adding widgets/popups, and notify
semantic content changes through `XmAccessibilityChanged`.

`provider.c` exercises native buttons/text and custom table/cell providers. Build
against the just-installed headers and library, with an absolute RPATH to that
prefix. Run the observer on an isolated Linux display and actual D-Bus session:

```
dbus-run-session -- python3 tests/accessibility/observe.py /absolute/provider /absolute/receipt.json
```

The observer requires `gdbus` as an external protocol client. The fixture itself
links no GLib, ATK, GTK or Qt. It verifies registration in the actual registry,
roles, an invoked button and changed name, focus acquisition, Unicode scalar text
slices and a custom table cell over the accessibility bus. An absent registry or
transport is a failing test, not a semantic-only substitute. Capture `ldd`, exact
source/library hashes and the actual fixture window as part of the host receipt.
Separately run without an accessibility bus to verify explicit unavailability and
normal widget operation; such a run never proves assistive-technology operation.

This is a foundation, not complete accessibility qualification. Native label,
button, toggle, text and container semantics and extensible providers are present.
AT-SPI Accessible/Application, actions, focus, scalar text reads and table cell
lookup have initial transport support. Unimplemented operations return an explicit
D-Bus NotSupported error. Additional Component operations, Text selection/editing/boundary
operations, table selections/headers, relation mapping and broader real
screen-reader journeys remain required. The development transport now supplies
the live Cache tree and structural add/remove events; its final installed-cohort
qualification remains separate from the source-linked checks below.
Do not claim complete AT-SPI interfaces, editing or screen-reader support from this
fixture. Source text positions are Unicode scalars in a Unicode locale; they are
not grapheme or visual-caret positions. Native text is converted from the active
locale to UTF-8; custom providers must return owned valid UTF-8 strings.

Native ScrollBars expose the AT-SPI ScrollBar role and Value interface. Values
come from `XmScrollBarGetValues`, including reversed processing directions.
`MinimumValue` is `XmNminimum`; `MaximumValue` is `XmNmaximum - XmNsliderSize`
(the last reachable thumb position); `MinimumIncrement` is `XmNincrement`.
Native setters accept finite integral values within those bounds, respect
`XmNeditable` and sensitivity, and call `XmScrollBarSetValues` with notification.
The six actions are numeric `increment`, `decrement`, `page-increment`,
`page-decrement`, `to-minimum` and `to-maximum`; they use the configured increments
and notify the ordinary valueChanged callback. Physical keyboard directions
continue through the native translations and respect `XmNprocessingDirection`.
Focusability and orientation reflect native traversal and orientation resources.
Native value/drag/step callbacks and Xt resource-change hooks publish current
Value properties and editable/enabled/sensitive/orientation/focus states. Hooks
are owned by the bridge and removed at teardown, including across attached
shells on different displays.

For custom scalar objects, zero-initialize `XmAccessibleProvider` and populate
`query` plus optional `set_value`. Query sets `has_value`, finite minimum,
maximum, current value and minimum increment, and optionally owned UTF-8
`value_text`. The range must be ordered, current must lie within it, and the
increment must be nonnegative; invalid projections fail the query. The enabled
and editable state bits control write admission. An absent setter is read-only.
The provider owns further validation and must call `XmAccessibilityChanged`
after changes. `XmAccessibilitySetValue` rejects nonfinite/out-of-range input
before calling it. `XmAccessibleInfoClear` releases the optional text.

`scrollbar.c` and `observe_scrollbar.py` add an actual registry/D-Bus regression:

```
xvfb-run -a dbus-run-session -- python3 tests/accessibility/observe_scrollbar.py /absolute/scrollbar /absolute/receipt.json
```

This observer additionally requires `xdotool` for real native Down/Right key
input. It verifies Value introspection/Get/GetAll/Set, scalar and reversed native
ranges, action names and results, actual native callbacks, focused/orientation
states, resource-change events, read-only/insensitive/invalid-value rejection,
continuous custom values, stale-object errors, and native operation after bridge
teardown. The installed-prefix runner includes it as `scrollbar-accessibility`.
These checks do not constitute broader screen-reader or Orca qualification.

The development lifecycle fixtures add ordered child/index lookup, stale IDs,
deep iterative teardown, provider reentrancy and bounded live-tree storage
(`lifecycle.c`); actual native focus and wrapped visual lines
(`native_geometry.c`); and independent D-Bus bulk Cache/structural events
(`tree_bus.c`, `observe_tree.py`). Compile the fixtures against one exact toolkit
prefix and run them on an isolated display. The tree observer takes the fixture
and output receipt as its two arguments, under `dbus-run-session`.

`native_controls.c` and `observe_orca.py` exercise actual Orca, Speech Dispatcher
and eSpeak on private display/bus/config/runtime endpoints. Supply the compiled
fixture, installed prefix, fresh absolute output directory and a short fresh
runtime path with `--fixture`, `--prefix`, `--out` and `--runtime`. The observer
records the actual process/library identities, utterances and generated PCM
without an audio device or user preference changes. Covered native controls
include buttons, toggles, editable/read-only Unicode text, selection, Disclosure
and ScrollBar. This subset does not qualify native list/menu items, precise edit
notifications, custom-provider layout, a complete application or human usability.


`text_events.c` and `observe_text_events.py` exercise native Text/TextField and
custom-provider text notifications over a private real AT-SPI bus. Compile the
fixture against the exact prefix, then run under an isolated display/session
with a fresh private `XDG_RUNTIME_DIR` (mode 0700). The observer requires the
AT-SPI socket returned by the private session bus to reside beneath that exact
runtime directory:

```
dbus-run-session -- python3 tests/accessibility/observe_text_events.py /absolute/text_events /absolute/receipt.json
```

Checks include scalar Unicode insertion/replacement/deletion payloads, combining
marks and supplementary characters, read-only programmatic edits, rejected edits,
caret-only/no-op/refresh suppression, changes across bounded read chunks, initial
baselines, provider removal/reentrancy/invalid UTF-8 recovery, and secret-provider
content suppression. These protocol assertions are separate from actual Orca
input-echo qualification. No documentation render is required for source review.

`menu.c` is a native semantics regression for menu-bar/pulldown/popup roles,
widget and gadget menu entries, shared-pane expansion ownership, MenuShell leaf
focus, native activation/popdown and rejected actions. Compile it against the
exact installed headers/library and run on an isolated X display. It does not
require a bus and does not establish D-Bus or screen-reader acceptance. It is executed by the native feature runner; actual D-Bus and screen-reader menu journeys remain separate.

The Linux-only `text_event_faults.c` is a test interposer, compiled as a shared
object with the libdbus headers and `libdl`. It delegates to actual libdbus and
injects one false/null return at a selected constructor or send operation of a
selected text event. Run `observe_text_faults.py` with the fixture executable,
interposer shared-object path and receipt path, on the same private display/bus
setup. Its 20 cases verify safe failed-container cleanup, no partial replacement
when construction fails, baseline invalidation after a failed send, and later
Unicode edit recovery. This is targeted protocol fault injection; it does not
simulate every toolkit/allocator failure. Custom-provider text ranges also use
Unicode scalars; the regular fixture destroys the bridge from a text callback
and checks that no content event escapes teardown.


`key_events.c` and `observe_key_events.py` exercise the actual registry's
synchronous, preemptive, non-global listener path with native X keyboard input.
Compile against the exact prefix and run the observer with fixture and receipt
paths under a private display/session and private `XDG_RUNTIME_DIR`, as above.
It requires `xdotool`, Python D-Bus and GI/GLib. The listener queries the native
text before processing a key, proves the same native F9 action before and after
selectively consuming it, observes an attached shell, injects a nested native key through an
accessible action, withholds a reply beyond the transport timeout, disconnects
without unregistering, and requests bridge destruction from a reentrant action.
Assertions check identity/string/modifiers/timestamp, once-only ordering,
normal native editing/translation, fail-open behavior, late-reply safety and
continued native operation after bridge destruction. This protocol fixture does
not substitute for actual screen-reader input-echo qualification or IME tests.

The bridge sends the registry parser's signed32 hardware/modifier input
`(uiiiisb)`. The real registry forwards unsigned32 fields to the listener as
`(uiuuisb)`; the observer explicitly checks the received Python D-Bus field types.
The receipt records both contracts and the pinned authoritative parser hash.
The first native key press/release and before-edit readback are the regression
for delivery: registry introspection alone incorrectly advertises unsigned input
and cannot establish this path. The signed-input correction passes the focused
installed-prefix ARM64 run; actual screen-reader echo and IME remain unqualified.

`list.c` / `observe_list.py` are a real private-registry List/Selection regression
for ABI7. They cover Unicode/repeated labels, row roles/names, selected child
readback, native focus/actions, insertion preserving identities, deletion and
identical-label replacement invalidating old IDs, whole-model reorder, extended
selection, disabled admission, native programmatic selection, generic provider
rejection, reentrant provider removal and widget destruction. They do not qualify
actual application navigation or Orca speech.

`list_faults.c` is a Linux/glibc-only test interposer. When explicitly armed by
the fixture it rejects calloc calls originating in libXm, while native List
storage remains on its ordinary Xt allocation path. The test inserts a native
item while the optional projection cannot allocate, verifies old row actions
fail closed, clears the fault and verifies exact identity/selection recovery.
It neither simulates all allocation failures nor modifies production allocator
policy. The installed feature runner builds/runs both regular and fault cases
against one exact ABI7 prefix. The fresh installed ABI7 native ARM64 regular and fault runs pass.


`selection_reentrancy.c` exercises generic provider count-time growth, owner
removal, selected-child replacement/removal/Changed notifications, duplicate IDs,
final-count mutation, refusal, accepted selection and post-mutation hierarchy
changes. It runs without the transport and checks that invalid reads never invoke
the mutation callback. `list_lifetime.c` runs eight native direct/semantic
selection cases in which ordinary callbacks destroy the List or shell, with and
without nested selection. The installed ABI7 pre-fix library deterministically
fails the count-growth getter assertion; normal native destruction runs may pass
without instrumentation, so that observation alone does not establish lifetime
safety. The corrected lifetime/provider fixtures also pass under ASan/UBSan. These
fixtures pass on the fresh installed ABI7 native ARM64 cohort (12 provider and
eight native destruction cases), as part of the full 20-fixture default suite.

The native runner selects `list-accessibility` and `list-allocation-faults`
independently. Its default suite includes both. Allocation fault interposition
uses the native libc allocator and must run without ASan; instrumented List
protocol/lifetime and provider-reentrancy fixtures run separately. The allocation
fixture rejects an ASan compiler rather than bypassing its allocator or relaxing
sanitizer runtime checks. Both scopes retain the same native behavior assertions.
