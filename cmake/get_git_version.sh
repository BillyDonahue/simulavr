#!/bin/sh
if which git > /dev/null 2> /dev/null; then
  H=$(git log --pretty=format:'%h' -n 1)
  D=$(git diff --quiet --exit-code || echo "+")
  T=$(git describe --abbrev=0)
  B=$(git rev-parse --abbrev-ref HEAD)
  V=$(echo "$T" | sed 's/[^0-9\.]//g')
else
  H="n/a"
  D=""
  T="n/a"
  V="0.0.0"
  B="n/a"
fi
echo "$H$D;$T;$V;$B"
