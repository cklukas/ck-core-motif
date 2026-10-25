# MWM menu child creation

Menu separators are widgets too and can fail to be created when the toolkit
cannot allocate a child.  `CreateMenuWidget` now checks every separator it
adds before counting it or passing the child array to `XtManageChildren`.
Successful menus retain the same separator types, names, and ordering.
