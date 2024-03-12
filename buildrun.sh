#!/bin/bash




cc -o client client.c xdg-shell-protocol.c -lwayland-client -lxkbcommon -Iinclude -lfreetype

# ./client