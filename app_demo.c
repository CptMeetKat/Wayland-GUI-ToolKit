#include "gui-textfield.h"
#include "client.h"

int main(int argc, char *argv[])
{
    struct client_state state = { 0 };
    init_surface(&state, 640, 480);

    registerComponent(&state, create_textfield(10, 10,"./fonts/DejaVuSansMono.ttf", 600, 100, "A demo of all working\ncomponents", BUFFER_SIZE, TEXT_COLOR, BACKGROUND_COLOR)->base);
    registerComponent(&state, create_textfield(10, 210, "./fonts/DejaVuSerif.ttf", 200, 200, "This is a textbox", BUFFER_SIZE, TEXT_COLOR, BACKGROUND_COLOR)->base);
    registerComponent(&state, create_textfield(220, 210, "./fonts/DejaVuSerif.ttf", 200, 200, "This is a second textbox", BUFFER_SIZE, TEXT_COLOR, BACKGROUND_COLOR)->base);
    registerComponent(&state, create_textfield(430, 210, "./fonts/DejaVuSerif.ttf", 200, 200, "This is a third\ntextbox", BUFFER_SIZE, TEXT_COLOR, BACKGROUND_COLOR)->base);

    while (wl_display_dispatch(state.wl_display)) {
        /* This space deliberately left blank */
    }

    return 0;
}
