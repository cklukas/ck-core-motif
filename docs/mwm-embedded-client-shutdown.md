# MWM embedded-client shutdown

Front-panel embedded clients can disappear before MWM shuts down. The
`UnParentControls` shutdown loop now skips records marked `CLIENT_DESTROYED`
before any save-set, reparent, or unmap request. Live embedded clients retain
the existing root-window restoration behavior.
