#!/bin/bash

echo "Generating files..."
autoreconf --install || {
  echo Error.
  exit 1
}

echo "done."
echo "Running configure..."
./configure "$@"
