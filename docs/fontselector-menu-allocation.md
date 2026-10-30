---
nav_exclude: true
---
# FontSelector encoding-menu allocation

The encoding option-menu builder now checks merged argument-list allocation and
each popup/pulldown/button creation before registering callbacks or applying
resources.  A partial new menu is destroyed without disturbing the existing
option menu, and the usable option-menu widget is returned.
