# MWM client icon and shape initialization

`iconMask` is populated from WM_HINTS, while `wShaped` is updated by the X
Shape event path. Both can be absent before the first frame is constructed.
`GetClientInfo` now initializes the mask to `None` and the shape flag to
`False`, preventing early frame/icon paths from consuming indeterminate state.
