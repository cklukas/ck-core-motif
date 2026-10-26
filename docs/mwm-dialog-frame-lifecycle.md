# MWM dialog frame lifecycle

Dialogs can be withdrawn or remanaged while their frame is being created or
torn down. `WithdrawDialog` and `ReManageDialog` now check
`clientFrameWin != None` before unmapping or mapping it, so lifecycle events
remain safe for a partially initialized client.
