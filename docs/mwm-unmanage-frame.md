# MWM unmanage frame withdrawal

Unmanage can run after a client has been registered but before its decoration
hierarchy is fully realized. The normal/maximized withdrawal path now checks
`clientFrameWin != None` before unmapping, so partial management cleanup is
safe while ordinary clients retain the same behavior.
