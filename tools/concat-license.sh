#!/bin/bash

LICENSE1_PATH="./LICENSE.txt"
LICENSE2_PATH="./submodules/luau/LICENSE.txt"
LICENSE3_PATH="./submodules/base64/LICENSE"
OUTPUT_FILE="./tmp/LICENSE.txt"

{
   echo -e "===== luaumb =====\n"
   cat "$LICENSE1_PATH"
   echo -e "\n===== luau =====\n"
   cat "$LICENSE2_PATH"
   echo -e "\n===== base64 =====\n"
   cat "$LICENSE3_PATH"
} > "$OUTPUT_FILE"
