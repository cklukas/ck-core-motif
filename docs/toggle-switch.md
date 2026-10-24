# ToggleSwitch

`XmToggleSwitch` is an additive binary control for applications that want a
compact on/off affordance without changing `XmToggleButton` behavior. Include
`<Xm/ToggleSwitch.h>` and create it with `XmCreateToggleSwitch` or one of its
variadic constructors.

The widget uses the normal Xt resource model:

- `XmNset` is the Boolean state.
- `XmNlabelString` supplies the label.
- `XmNvalueChangedCallback` receives an `XmToggleSwitchCallbackStruct` whose
  `set` member is the new state. The callback may destroy the widget; callers
  must not use the widget after returning from the callback.
- `XmToggleSwitchGetState` and `XmToggleSwitchSetState` provide typed state
  access. `notify=False` changes state without invoking callbacks.

Space, Return, and primary-button release toggle the state when the widget is
sensitive. Focus traversal is inherited from `XmPrimitive`; the focus ring is
drawn separately from the state indicator, so the control remains
understandable without relying on color alone.

When an `XmAccessibility` bridge is attached, the control exposes the
AT-SPI check-box role, its label as the accessible name, the checked state, a
single `activate` action, and value-change notifications. Queries and actions
remain on the owning Xt application thread.

The installed consumer fixture
`tests/automated/run-toggle-switch.py` covers construction, labels, state
notification, no-notify updates, keyboard focus, disabled rendering state,
accessibility query/action, resize metrics, and destruction from a value
callback under Xvfb.
