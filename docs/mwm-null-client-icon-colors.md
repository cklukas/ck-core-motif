# WSM null-client icon colors

`MakeIconPixmap` supports constructing an icon from screen defaults without a
client. Its masked-image color path now selects `iconAppearance` from the
screen when no client exists, preserving valid foreground/background values
for built-in icons.
