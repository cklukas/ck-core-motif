# MWM stretcher-window creation rollback

Resize handles are separate X input-only windows.  Previously, a failed
`XCreateWindow` left the frame construction running with only a subset of its
resize handles, producing a partially functional decoration.  Stretcher
creation is now transactional: any failure destroys every handle created for
that frame and causes `ConstructFrame` to unwind the frame window.

Normal frames still create the same handles, while low-resource failures no
longer leave a half-working resize border.
