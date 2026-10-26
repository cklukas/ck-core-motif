# MWM icon-frame window allocation

When an icon is displayed outside the icon box, `MakeIcon` creates its frame
window before allocating shadow geometry and repainting the icon. The
constructor now checks that allocation for both normal and WSM builds. A
failure returns `False` immediately, so later icon operations never target an
invalid X window.
