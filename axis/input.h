#ifndef INPUT_H
#define INPUT_H

#include <ultra64.h>
#include <PR/sched.h>
#include "printf.h"
#include "math/vector.h"
#include "stdint.h"

/* TODO: I should write a macro to convert usec to floating point time, then change all of the time values in here to use floats
 * TODO: button_?_t might be a bad naming scheme for controller buttons. Might want to rename these and move into their own header
 */

// bit index for each button
enum button_index {
    BUTTON_INDEX_CRIGHT,
    BUTTON_INDEX_CLEFT,
    BUTTON_INDEX_CUP,
    BUTTON_INDEX_CDOWN,
    BUTTON_INDEX_RT,
    BUTTON_INDEX_LT,
    BUTTON_INDEX_PAD0,
    BUTTON_INDEX_PAD1,
    BUTTON_INDEX_DRIGHT,
    BUTTON_INDEX_DLEFT,
    BUTTON_INDEX_DDOWN,
    BUTTON_INDEX_DUP,
    BUTTON_INDEX_START,
    BUTTON_INDEX_Z,
    BUTTON_INDEX_B,
    BUTTON_INDEX_A,
    BUTTON_INDEX_COUNT
};

typedef enum {
    BUTTON_STATE_UP,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_DOWN
} button_state_t;

typedef struct {
    button_state_t state;
    uint64_t time; // time pressed, otherwise 0
} button_t;

typedef struct
{
    button_t buttons[BUTTON_INDEX_COUNT];
    vec2f_t stick_now;
    vec2f_t stick_before;
} controller_t;


typedef struct {
    OSMesgQueue controller_message_queue, serial_message_queue;
    OSMesg controller_message_buffer, serial_message_buffer;
    OSContStatus controller_status[4];
    OSContPad controller_now[4];
    controller_t controller[4]; // TODO: think of a better name
    uint8_t controller_valid;
    uint8_t controller_read;
} input_context_t __attribute__((aligned(8)));

void controller_construct(controller_t* cont) {
    uint32_t index;

    for (index = 0; index < BUTTON_INDEX_COUNT; index++) {
        cont->buttons[index].state = BUTTON_STATE_UP;
        cont->buttons[index].time = 0;
    }

    cont->stick_now = vec2f_zero;
    cont->stick_before = vec2f_zero;
}

void input_context_construct(input_context_t* input) {
    uint32_t index;

    // it works and I will leave it at that
    osCreateMesgQueue(&input->serial_message_queue, &input->serial_message_buffer, 1);
    osSetEventMesg(OS_EVENT_SI, &input->serial_message_queue, 1);

	osContInit(&input->serial_message_queue, &input->controller_valid, &input->controller_status[0]);

    osCreateMesgQueue(&input->controller_message_queue, &input->controller_message_buffer, 1);
	osSetEventMesg(OS_EVENT_SI, &input->controller_message_queue, 0);

    for (index = 0; index < 4; index++) {
        controller_construct(&input->controller[index]);
    }
}

void input_context_step(input_context_t* input) {
    uint32_t index;
    uint32_t button_index;
    uint32_t button_pressed;
    button_t* this_button;

    osContGetReadData(&input->controller_now[0]);
    input->controller_read = 0;

    for (index = 0; index < 4; index++) {
        if (input->controller_now[index].errno == 0) {
            for (button_index = 0; button_index < BUTTON_INDEX_COUNT; button_index++) {
                this_button = &input->controller[index].buttons[button_index];
                button_pressed = (input->controller_now[index].button & (1 << button_index)) != 0;

                switch (this_button->state)
                {
                case BUTTON_STATE_UP:
                    if (button_pressed) {
                        this_button->state = BUTTON_STATE_PRESSED;
                        this_button->time = 0;
                    }
                    break;

                case BUTTON_STATE_PRESSED:
                    this_button->time = osGetTime();

                    if (button_pressed) this_button->state = BUTTON_STATE_DOWN; // ++
                    else this_button->state = BUTTON_STATE_RELEASED; // --
                    break;

                case BUTTON_STATE_DOWN:
                    if (!button_pressed) this_button->state = BUTTON_STATE_RELEASED;
                    break;

                case BUTTON_STATE_RELEASED:
                    if (button_pressed) this_button->state = BUTTON_STATE_PRESSED;
                    else this_button->state = BUTTON_STATE_UP;
                    break;
                }
            }

            input->controller[index].stick_before = input->controller[index].stick_now;

            // TODO: deadzone & peakzone
            input->controller[index].stick_now.x = (float)input->controller_now[index].stick_x * 0.0078125f;
            input->controller[index].stick_now.y = (float)input->controller_now[index].stick_y * 0.0078125f;
        }
    }
}

#endif /* INPUT_H */

