# GrabShell child-count bounds

`_XmFastExpose` traverses the GrabShell composite child array using the Xt
`Cardinal` child-count type. Its index now uses `Cardinal`, preserving expose
ordering while avoiding signed-index wraparound for large child lists.
