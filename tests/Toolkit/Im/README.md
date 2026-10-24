# Native input-method service qualification

The separate `local_compose.c`, `local_compose.py` and `local_compose.Compose`
fixture uses actual local XIM lookup without a provider service. The observer
accepts an already compiled native fixture, a fresh absolute output directory
and `--prefix /absolute/installed/prefix`; it owns its authenticated Xvfb/Openbox.
It fingerprints its standalone Compose input and private keymap, sends XTest
Compose/dead-key sequences under both input-sharing policies, and checks native
bytes, scalar carets, committed-value callbacks, focus isolation and grapheme
motion/deletion. It restores the keymap and closes only its owned processes.
This is a source test contract, not a recorded runtime pass; Escape, external
providers and physical keyboard behavior are separate scopes.

`service.c` and `service.py` exercise real IBus/m17n XIM composition in native
Text and TextField widgets. Run only inside an owned private X server and
`dbus-run-session`, with explicit task TMPDIR and fresh XDG state. The fixture
requires IBus/m17n, XTest/xdotool, Python GI, and the IBus 1.0 typelib. Keep
`ibus_engine.py` beside `service.py`; the receipt fingerprints that helper.

Engine selection connects to the explicit private abstract IBus endpoint. It
resolves the unique provider owner, calls `SetGlobalEngine`, independently reads
and deserializes `GlobalEngine`, and checks that the provider owner stayed the
same. Setter errors and getter errors are recorded separately; an old matching
property cannot stand in for an accepted setter call. Connect, calls and
transport close share a bounded cancellation deadline.

The IBus 1.5.32 setter returns a void D-Bus reply; libIBus represents receipt of
that reply as boolean success. The `GlobalEngine` property is itself a variant
containing a serialized EngineDesc. These contracts follow
[ibusbus.c](https://github.com/ibus/ibus/blob/1.5.32/src/ibusbus.c) and
[ibusimpl.c](https://github.com/ibus/ibus/blob/1.5.32/bus/ibusimpl.c).
The helper uses this API directly because the
[CLI engine setter](https://github.com/ibus/ibus/blob/1.5.32/tools/main.vala)
can also invoke `setxkbmap`, and its exit status includes that separate step.
The helper does not invoke the CLI or change keyboard layouts.

Global-engine readback does not identify the engine attached to a particular
widget or InputContext. Native preedit/commit/focus/restart assertions remain
the composition gate. Isolated API admission checks do not establish composition
acceptance, resolve intermittent provider-boundary failures, or replace those
assertions. Diagnostic instrumentation and engine-global observations must not
be presented as per-context protocol evidence.
