#!/bin/bash


cc -o client \
   client.c \
   ./include/xdg-shell-protocol.c \
   ./include/widget/gui-widget.c \
   ./include/gui-textfield/gui-textfield.c \
   ./include/gui-textfield/cursor.c \
   ./include/command/command.c \
   ./include/gui-textfield/command_insert.c \
   ./include/history/history.c \
   ./include/gui-textfield/command_remove.c \
   ./include/stack/stack.c \
   ./include/deque/deque.c \
   ./include/gap_buffer/gap_buffer.c \
   -lwayland-client \
   -lxkbcommon \
   -Iinclude \
   -lfreetype \
   -Iinclude/gap_buffer \
   -Iinclude/deque \
   -Iinclude/stack \
   -Iinclude/command \
   -Iinclude/history \
   -Iinclude/gui-textfield \
   -Iinclude/widget 

if [ $? -eq 0 ]; then
    valgrind --leak-check=full -s ./client
fi
