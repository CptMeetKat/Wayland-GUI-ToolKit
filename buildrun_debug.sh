#!/bin/bash


cc -g -o client \
   client.c \
   xdg-shell-protocol.c \
   gui-widget.c \
   gui-textfield.c \
   cursor.c \
   command.c \
   command_insert.c \
   history.c \
   command_remove.c \
   ./include/deque/deque.c \
   ./include/gap_buffer/gap_buffer.c \
   -lwayland-client \
   -lxkbcommon \
   -Iinclude \
   -lfreetype \
   -Iinclude/gap_buffer \
   -Iinclude/deque

if [ $? -eq 0 ]; then
    valgrind --leak-check=full -s ./client
fi