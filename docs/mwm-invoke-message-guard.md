# MWM invoke-message guard

Command invocation messages use the per-screen UTM drawing area.  If that
optional widget was unavailable, `SendInvokeMessage` now frees its prepared
payload and returns without calling the UTM transport.  This keeps command
handling safe while preserving normal message delivery when UTM is present.
