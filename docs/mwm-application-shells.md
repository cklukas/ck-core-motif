# MWM application shells

MWM now validates creation of its application shell and, in workspace-manager
builds, the secondary display connection and shell.  Failed creation stops
startup before color-object initialization, event registration, or resource
updates can use a null shell.  Successful shell classes and resources remain
unchanged.
