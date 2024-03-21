#!/bin/bash


cc -o client client.c xdg-shell-protocol.c gui-widget.c gui-textfield.c timed-event.c -lwayland-client -lxkbcommon -Iinclude -lfreetype
if [ $? -eq 0 ]; then
    ./client
fi
