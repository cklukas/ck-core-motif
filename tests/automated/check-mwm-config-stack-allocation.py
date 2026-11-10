#!/usr/bin/env python3
"""Check MWM does not continue after configuration-stack allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
prototype = "static Boolean ConfigStackInit (char *pchFileName);"
assert prototype in source

start = source.index("static Boolean ConfigStackInit (char *pchFileName)",
                     source.index(prototype) + len(prototype))
end = source.index("\n\f", start)
initializer = source[start:end]
assert "ConfigFileStackEntry *stack;" in initializer
assert "stack->fileName = XtNewString(pchFileName);" in initializer
assert "if (stack->fileName == NULL)" in initializer
assert "XtFree((char *)stack);" in initializer
assert "return False;" in initializer
assert initializer.index("pConfigStack = stack;") > initializer.index(
    "if (stack->fileName == NULL)")

caller = source[source.index("if (!pConfigStack && !ConfigStackInit(cfileName))"):]
assert "fclose(fileP);" in caller[:500]
assert "return NULL;" in caller[:500]

print("PASS MWM configuration-stack allocation failure is transactional")
