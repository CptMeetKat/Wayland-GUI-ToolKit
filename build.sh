#!/bin/bash

# Function to display usage information
usage() {
    echo "Usage: $0 [-c | -d | -b]"
    echo "  -c: compile"
    echo "  -d: debug and run"
    echo "  -b: build and run"
    exit 1
}

compile()
{
   cc -o app \
      app.c \
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
}

run()
{
    if [ $? -eq 0 ]; then
        ./app
    fi
}

buildrun()
{
    compile;
    run;
}


buildrun_demo()
{
   cc -o app_demo \
      app_demo.c \
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
        ./app_demo
    fi
}

debugrun()
{
   cc -g -o app \
      app.c \
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
      valgrind --leak-check=full -s ./app
    fi
}

# Check if no arguments provided, display usage
if [ $# -eq 0 ]; then
    buildrun
fi

# Parse command line options
while getopts ":dcbht" opt; do
    case ${opt} in
        d)
            echo "Debug and run..."
            debugrun
            ;;
        c)
            echo "Compiling..."
            compile
            ;;
        b)
            echo "Build and run"
            buildrun
            ;;
        t)
            buildrun_demo
            ;;
        h)
            usage
            ;;
        \?)
            # If unknown option
            echo "Invalid option: $OPTARG"
            usage
            ;;
    esac
done

