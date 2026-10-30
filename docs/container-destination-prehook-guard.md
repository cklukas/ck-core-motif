# Container destination prehook validation

`ContainerDestPrehookProc` now rejects callbacks without a transfer handle
before allocating location data or invoking transfer completion APIs. This
keeps malformed or stale destination callbacks from reaching the transfer
machinery with a null identifier.
