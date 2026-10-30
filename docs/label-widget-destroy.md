# Badge, Link, and Prompt label destruction

Badge, Link, and Prompt copy their `XmString` label/text resources so that
resource replacement is transactional. Their destructors now release those
owned copies, preventing leaks across repeated widget creation and destruction.
