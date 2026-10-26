# MWM client base-window initialization

`clientBaseWin` is created only after the frame and gadget geometry are ready.
`GetClientInfo` now initializes it to `None`, so frame cleanup and regeneration
can reliably distinguish an uncreated base window from an arbitrary heap value.
