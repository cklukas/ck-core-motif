# MWM frame regeneration

Frame regeneration can be requested during client teardown or after an
allocation failure. `RegenerateClientFrame` now validates the frame, optional
title/base children, and all eight resize-cursor windows before reconfiguring
them. Partial hierarchies are left untouched instead of producing invalid X
requests.
