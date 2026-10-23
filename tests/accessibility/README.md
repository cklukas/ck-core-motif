# Optional native accessibility foundation

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
operations, table selections/headers, relation mapping, bulk Cache, automatic
structural event completeness and real screen-reader journeys remain required.
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
