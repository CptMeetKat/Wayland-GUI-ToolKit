#include "gui-textfield.h"
#include "client.h"

int main(int argc, char *argv[])
{

    int bg_color = 0x001A1B26;
    int text_color = 0xFFFFFFFF;


    struct client_state state = { 0 };
    init_surface(&state, 640, 480, bg_color);

    registerComponent(&state, create_textfield(10, 10,"./fonts/DejaVuSansMono.ttf", 600, 200, "This is what you should do\nFollow\nthe white\nrabbit..............", BUFFER_SIZE, text_color, bg_color)->base);
    registerComponent(&state, create_textfield(10, 210, "./fonts/DejaVuSerif.ttf", 200, 220, "HeyZukoHere", BUFFER_SIZE, text_color, bg_color)->base);
//    registerComponent(&state, create_textfield(350, 80, "DejaVuSerif.ttf", 200, 220, "", BUFFER_SIZE)->base);
//    registerComponent(&state, create_textfield(400, 10, "DejaVuSerif.ttf", 26, 200, "aW", BUFFER_SIZE)->base);
    //registerComponent(&state, create_textfield(400, 220, "DejaVuSerif.ttf", 2, 100, "aW", BUFFER_SIZE)->base);
//    registerComponent(&state, create_textfield(10, 300, "DejaVuSerif.ttf", 300, 200, "aW", BUFFER_SIZE)->base);
//    registerComponent(&state, create_textfield(450, 300, "DejaVuSerif.ttf", 300, 200, "aW", BUFFER_SIZE)->base);
//    registerComponent(&state, create_textfield(450, -100, "DejaVuSerif.ttf", 300, 200, "aW", BUFFER_SIZE)->base);
//    registerComponent(&state, create_textfield(-100, 450, "DejaVuSerif.ttf", 300, 200, "aW", BUFFER_SIZE)->base);

    while (wl_display_dispatch(state.wl_display)) {
        /* This space deliberately left blank */
    }

    return 0;
}
