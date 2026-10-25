# MWM startup widget creation

Screen and workspace resource widgets are created during MWM startup.  The
startup path now stops safely when a required screen shell or workspace widget
cannot be allocated, and it skips optional UTM callback setup when its drawing
area cannot be created.  Successful startup retains the same widget classes,
names, resources, and popup behavior.
