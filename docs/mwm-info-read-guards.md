# MWM info read guards

`GetMwmInfo` now accepts the root `_MOTIF_WM_INFO` property only when its
format is 32-bit and its item count matches the expected structure. Invalid
properties are released and treated as absent.
