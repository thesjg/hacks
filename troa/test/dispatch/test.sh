#!/bin/sh
echo "nodispatch"
time ./nodispatch >/dev/null

echo "nodispatch (inline)"
time ./nodispatch-inline >/dev/null

echo "base"
time ./base >/dev/null

echo "goto direct"
time ./goto-direct >/dev/null

echo "goto direct 2"
time ./goto-direct2 >/dev/null

echo "goto direct switch"
time ./goto-direct-switch >/dev/null

echo "scary goto"
time ./scarygoto >/dev/null

echo "scary goto (inline)"
time ./scarygoto-inline >/dev/null

echo "goto"
time ./goto >/dev/null

echo "modified goto"
time ./mgoto >/dev/null

echo "modified goto 1"
time ./mgoto1 >/dev/null

echo "modified goto 2"
time ./mgoto2 >/dev/null

echo "switch"
time ./switch >/dev/null

echo "switch direct"
time ./switch-direct >/dev/null

echo "switch direct inline"
time ./switch-direct-inline >/dev/null

echo "direct (stack exploder)"
time ./direct >/dev/null

echo "modified direct (stack exploder)"
time ./mdirect >/dev/null

echo "function pointer"
time ./funcptr >/dev/null

echo "modified function pointer"
time ./mfuncptr >/dev/null

echo "call (nodispatch)"
time ./call-nodispatch >/dev/null

echo "call (switch)"
time ./call-switch >/dev/null
