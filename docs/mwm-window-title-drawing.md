# MWM window-title drawing

`DrawWindowTitle` may draw into either the client frame or its separate title
child, depending on appearance settings. It now validates the selected XID
after that choice, so title exposure during partial teardown returns cleanly.
