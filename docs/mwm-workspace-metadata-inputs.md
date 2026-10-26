# MWM workspace metadata inputs

The WSM workspace metadata helpers now reject incomplete records safely:

- `SetWorkspaceInfoProperty(NULL)` and a workspace without `pSD` return before
  constructing or publishing a property.
- `DeleteWorkspaceInfoProperty(NULL)` and a workspace without `pSD` return
  before atom lookup or deletion.
- `WorkspacePropertyName(NULL)` and a workspace with no `name` return `NULL`.

Complete workspace records keep the existing property format and X11 behavior.
