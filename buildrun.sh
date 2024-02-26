#!/bin/bash




cc -o client client.c xdg-shell-protocol.c bitmap.c -lwayland-client -lxkbcommon -Iinclude -lfreetype

./client