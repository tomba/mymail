#!/bin/bash

valgrind --suppressions=valgrind-mymail.supp --leak-check=full --show-leak-kinds=definite --gen-suppressions=yes ./build/Debug/mymail $*
