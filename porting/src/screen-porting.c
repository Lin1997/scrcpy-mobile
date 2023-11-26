//
//  screen-porting.c
//  scrcpy-module
//
//  Created by Ethan on 2022/6/3.
//

#include "stdbool.h"

bool ScrcpyEnableHardwareDecoding(void);

#define sc_screen_init(...)   sc_screen_init_orig(__VA_ARGS__)
#define sc_frame_buffer_consume(...)   sc_frame_buffer_consume_hijack(__VA_ARGS__)
#define sc_screen_handle_event(...)    sc_screen_handle_event_hijack(__VA_ARGS__)

#include "screen.c"

#undef sc_screen_init
#undef sc_frame_buffer_consume
#undef sc_screen_handle_event

struct sc_screen *
sc_screen_current_screen(struct sc_screen *screen) {
    static struct sc_screen *current_screen;
    if (screen != NULL) {
        current_screen = screen;
    }
    return current_screen;
}

__attribute__((weak))
float screen_scale(void) {
    return 2.f;
}

bool
sc_screen_init(struct sc_screen *screen,
               const struct sc_screen_params *params) {
    bool ret = sc_screen_init_orig(screen, params);

    // Set renderer scale
    SDL_RenderSetScale(screen->display.renderer, screen_scale(), screen_scale());
    
    // Save current screen pointer
    sc_screen_current_screen(screen);

    return ret;
}

__attribute__((weak))
void ScrcpyHandleFrame(AVFrame *frame) {}

void
sc_frame_buffer_consume(struct sc_frame_buffer *fb, AVFrame *dst);
// Hijack sc_video_buffer_consume to convert NV12 pixels
void
sc_frame_buffer_consume_hijack(struct sc_frame_buffer *fb, AVFrame *dst) {
    sc_frame_buffer_consume(fb, dst);
    
    // Handle hardware frame render
    if (ScrcpyEnableHardwareDecoding()) ScrcpyHandleFrame(dst);
}

bool
sc_screen_handle_event(struct sc_screen *screen, SDL_Event *event) {
    // Handle Clipboard Event to Sync Clipboard to Remote
    if (event->type == SDL_CLIPBOARDUPDATE) {
        char *text = SDL_GetClipboardText();
        if (!text) {
            LOGW("Could not get clipboard text: %s", SDL_GetError());
            return false;
        }

        char *text_dup = strdup(text);
        SDL_free(text);
        if (!text_dup) {
            LOGW("Could not strdup input text");
            return false;
        }

        struct sc_control_msg msg;
        msg.type = SC_CONTROL_MSG_TYPE_SET_CLIPBOARD;
        msg.set_clipboard.sequence = SC_SEQUENCE_INVALID;
        msg.set_clipboard.text = text_dup;
        msg.set_clipboard.paste = false;

        if (!sc_controller_push_msg(screen->im.controller, &msg)) {
            free(text_dup);
            LOGW("Could not request 'set device clipboard'");
            return false;
        }
        return true;
    }
    
    return sc_screen_handle_event_hijack(screen, event);
}
