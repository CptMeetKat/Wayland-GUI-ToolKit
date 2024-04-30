#ifndef CLIENT_H
#define CLIENT_H


#define MAX_COMPONENTS 32
#define BUFFER_SIZE 128 //Need to make this dynamic
/* Wayland code */
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>




struct pointer_event {
    uint32_t event_mask;
    wl_fixed_t surface_x, surface_y;
    uint32_t button, state;
    uint32_t time;
    uint32_t serial;
    struct {
        bool valid;
        wl_fixed_t value;
        int32_t discrete;
    } axes[2];
    uint32_t axis_source;
};

struct client_state {
    /* Globals */
    struct wl_display *wl_display;
    struct wl_registry *wl_registry;
    struct wl_shm *wl_shm;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base *xdg_wm_base;
    struct wl_seat *wl_seat;
    /* Objects */
    struct wl_surface *wl_surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct wl_keyboard *wl_keyboard;
    struct wl_pointer *wl_pointer;
    struct wl_touch *wl_touch;
    float offset;
    uint32_t last_frame;
    int width, height;
    bool closed;
    struct pointer_event pointer_event;
    struct xkb_state *xkb_state;
    struct xkb_context *xkb_context;
    struct xkb_keymap *xkb_keymap;

    //components
    struct Widget *components[MAX_COMPONENTS];
    int total_components;

    struct Widget* focused;
    int focused_index;

    int rate;
    int delay;

    int last_key_action;
    int last_key_time;
    int last_key;

    int modifier;

    int background_color;
};


void init_surface(struct client_state* state, int width, int height, int background_color);
void registerComponent(struct client_state *state, struct Widget* w);



#endif
