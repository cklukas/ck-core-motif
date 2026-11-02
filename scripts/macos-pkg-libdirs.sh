#!/bin/sh
# Resolve pkg-config choices before a broad XQuartz -L path can override them.
set -eu
pkg=$1
shift
x11=$("$pkg" --variable=libdir x11)
x11=$(cd -P "$x11" && pwd)
emitted=""
for package in "$@"; do
flags=$("$pkg" --libs "$package")
directories=""
for flag in $flags; do
    case $flag in -L*) directories="$directories ${flag#-L}";; esac
done
for flag in $flags; do
    case $flag in
    -l*)
        name=${flag#-l}
        for directory in $directories; do
            path=$directory/lib$name.dylib
            test -f "$path" || continue
            while test -L "$path"; do
                target=$(readlink "$path")
                case $target in /*) path=$target;; *) path=$(dirname "$path")/$target;; esac
            done
            directory=$(cd -P "$(dirname "$path")" && pwd)
            # XQuartz's broad directory belongs after the narrow package dirs.
            if test "$directory" != "$x11"; then
                case " $emitted " in *" $directory "*) ;; *)
                    printf '%s ' "-L$directory"
                    emitted="$emitted $directory"
                    ;;
                esac
            fi
            break
        done
        ;;
    esac
done
done
printf '\n'
