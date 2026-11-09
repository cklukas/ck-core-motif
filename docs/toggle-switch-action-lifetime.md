# ToggleSwitch action callback lifetime

The ToggleSwitch action now delegates redraw to
`XmToggleSwitchSetState` and does not touch the widget after the value-changed
callbacks run. A callback is allowed to destroy the widget; the action no
longer performs a redundant redraw through that potentially stale handle.

Pointer release and hover actions apply the same lifetime rule before they
change the armed or hover state.  Late X events delivered while Xt is
destroying the ToggleSwitch are ignored rather than requesting a redraw or
changing widget state during teardown.
