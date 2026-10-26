# MWM active icon-label painting

`PaintActiveIconText` draws the optional label's shadow and text. It now checks
the client/screen pointers and helper-window ID before reading appearance data,
so exposure callbacks after helper-window teardown cannot issue invalid X
requests.
