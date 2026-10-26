# MWM icon-title geometry helper

`GetIconTitleBox` computes the rectangle used by icon-title painting. It now
returns immediately for a null client, missing screen state, or null output
rectangle, keeping geometry calculation safe when callbacks race with teardown.
