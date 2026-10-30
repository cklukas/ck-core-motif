# ToggleSwitch action callback lifetime

The ToggleSwitch action now delegates redraw to
`XmToggleSwitchSetState` and does not touch the widget after the value-changed
callbacks run. A callback is allowed to destroy the widget; the action no
longer performs a redundant redraw through that potentially stale handle.
