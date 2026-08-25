/*
 * ===========================================
 * Copyright(c) 2026-present IgosProjects
 * Copyright(c) 2026-present TheGooseFoundation and contributors
 * 
 * GooseOS is licensed under the GNU General Public License version 3(GPLv3), 
 * the license text can be found inside of the LICENSE file at the root of the repositoy
 *
 * If you didnt receive the LICENSE file with this source code, 
 * it can be found at https://www.gnu.org/licenses/gpl-3.0.html
 * ==========================================
 */

// Flanterm includes, used to initilize Flanterm(the console emulator)
#include <extern/flanterm/flanterm.h>
#include <extern/flanterm/fb.h>

#include <drivers/uart.h>
#include <stdbool.h>
#include <extern/tinyprintf/tinyprintf.h>
#include <utils/boot/info.h>
#include <utils/boot/limine.h>
#include <drivers/kcon.h>
#include <assert.h>
#include <mm/alloc.h>

#include <stdarg.h>

struct GFramebuffer* global_fb;
struct flanterm_context* global_ctx;

bool kcon_initilized = false;

/// @brief Initilizes the console driver allowing for characters to be printed to the display
/// @attention Call only ONCE on boot!
void kcon_init(struct GFramebuffer* fb) {
    // Check if we got a valid framebuffer sent in, this is important to prevent failures later on!
    //ASSERT(fb == NULL);

    // Now we have CONFIRMED that the framebuffer exists, lets set the "global_fb"
    global_fb = fb;

    // Now we need to initilize Flanterm, Flanterm is our console emulator we use it to print characters to the display
    global_ctx = flanterm_fb_init(
        NULL,
        NULL,
        (uint32_t*)fb->address, fb->width, fb->height, fb->pitch,
        fb->red_mask_size, fb->red_mask_shift,
        fb->green_mask_size, fb->green_mask_shift,
        fb->blue_mask_size, fb->blue_mask_shift,
        NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0,
        0,
        true
    );

    uart_init(0x3F8); // Initilize serial UART on COM1

    kcon_initilized = true;
}

/// @brief Prints a single character to the display, replacement for the standard C function "putc"
/// @param c: The character to print
void kputc(const char c) {
    if (!kcon_initilized || !global_ctx) return;

    uart_putc(c);
    flanterm_write(global_ctx, &c, 1); // Tell Flanterm to draw our character to the display
}

/// @internal This function is internally called and is not a part of the public API!
/// @internal Its used for tinyprintf since it passes 2 arguments instead of one
static void _kputc(void* ignored, const char c) {
    kputc(c);
}

/// @brief Prints a formatted string to the display, replacement for the standard C function "printf"
/// @param str: The string to print to the display
/// @param ...: Formatting to use
void kprintf(const char* str, ...) {
    if (!kcon_initilized) return;

    // Since we use tinyprintf, we need to call its functions
    // They expect a VAList!
    va_list args;
    va_start(args, str);

    tfp_format(NULL, _kputc, str, args);

    va_end(args);
}

/// @brief Same as "kprintf()" but it accepts a direct VAList
/// @param str: The formatted string to print
/// @param args: Arguments to use 
void kvsnprintf(const char* str, va_list args) {
    tfp_format(NULL, _kputc, str, args); // This is just simpler kprintf lol
}