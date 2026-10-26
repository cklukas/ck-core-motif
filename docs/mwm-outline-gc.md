# MWM outline drawing and failed GC creation

`FlashOutline` and `DrawOutline` draw the temporary move/resize outline with
the screen's XOR graphics context. X resource allocation can fail, leaving
that context unavailable. Both entry points now return before issuing an X
drawing request when the active screen record or its XOR GC is missing. This
keeps a resource failure from becoming a `BadGC` request while preserving the
normal outline behavior when initialization succeeds.
