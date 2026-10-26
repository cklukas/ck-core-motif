# MWM title-bar width bounds

The title child width is the frame width minus both upper borders.  For a
very narrow frame that unsigned subtraction could wrap and request an
enormous X window.  Frame construction now rejects such geometry, and later
frame reconfiguration skips the invalid title resize instead of issuing a
wrapped request.

Normal frames use the unchanged title-bar geometry.
