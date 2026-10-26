# MWM frame-hierarchy IDs

Destroying a client frame also destroys its title and base child windows. The
teardown helper now clears all three cached IDs (`clientFrameWin`,
`clientTitleWin`, and `clientBaseWin`) together, keeping the client record
consistent with the X server and making repeated lifecycle operations safe.
