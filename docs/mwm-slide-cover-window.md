# MWM slide cover-window creation failure

Slide-up transitions use a temporary input-only cover window while the client
frame moves. If that optional window cannot be created, the transition now
clears its pending slide record and returns. The client remains mapped without
issuing configure/map requests against an invalid `None` window.
