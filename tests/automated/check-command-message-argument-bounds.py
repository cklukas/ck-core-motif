#!/usr/bin/env python3
from pathlib import Path

command = Path("lib/Xm/Command.c").read_text()
message = Path("lib/Xm/MessageB.c").read_text()

assert "(size_t)ac >= SIZE_MAX / sizeof(Arg)" in command
assert "ac >= (Cardinal)(UINT_MAX / sizeof(Arg))" in command
assert "(ac != 0 && al == NULL)" in command
assert "if (ac != 0)\n\tmemcpy(argsNew, al, sizeof(Arg) * (size_t)ac);" in command

assert "(size_t)ac >= SIZE_MAX / sizeof(Arg)" in message
assert "(unsigned int)ac >= UINT_MAX / sizeof(Arg)" in message
assert "(ac != 0 && al == NULL)" in message
assert "if (ac != 0)\n\tmemcpy(argsNew, al, sizeof(Arg) * (size_t)ac);" in message

print("Command and message dialog argument merges check byte bounds and null lists")
