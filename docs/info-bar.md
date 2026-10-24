# InfoBar

`XmInfoBar` is an additive inline information, warning or error banner. Its
`XmNinfoSeverity` and `XmNinfoMessage` are explicit resources; an optional
`XmNinfoActionLabel` invokes `XmNinfoActionCallback`, and
`XmNinfoClosable` enables Escape or close-glyph dismissal through
`XmNinfoDismissCallback`. The bar never grabs input or moves keyboard focus.
Its accessible representation exposes the message text as a live panel.
Existing MessageBox, dialog and application status-area behavior is unchanged.
