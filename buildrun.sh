#!/bin/bash




cc -o client client.c xdg-shell-protocol.c gui-widget.c gui-textfield.c -lwayland-client -lxkbcommon -Iinclude -lfreetype

./client
