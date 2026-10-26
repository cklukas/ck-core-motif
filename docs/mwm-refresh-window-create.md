# MWM refresh-window creation failure

The non-WSM refresh commands use short-lived windows to trigger repainting.
Both `F_Refresh` and `F_Refresh_Win` now check the temporary
`XCreateWindow` result before mapping and destroying it. If allocation fails,
the command returns successfully without issuing requests against `None`.
