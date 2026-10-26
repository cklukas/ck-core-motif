# MWM client refresh base-window guard

`F_Refresh_Win` creates a temporary child of the client's base window.  During
unmanage and frame teardown, callbacks can arrive after `clientBaseWin` has
been cleared.  The refresh action now returns without issuing an X request when
that base window is `None`.

Normal managed clients retain the existing temporary refresh behavior.
