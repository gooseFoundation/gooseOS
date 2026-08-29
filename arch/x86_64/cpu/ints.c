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

#include <stdint.h>
#include <kernel/panic.h>
#include <drivers/kcon.h>

/// @brief Interrupt frame that we push onto the stack, only ever used by x86_64!
/// @internal
struct interrupt_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

    uint64_t int_no;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
} __attribute__((packed));

/// @brief Internally called function used to handle ISR exceptions
/// @internal This function is called only internally and used by the early ISR entries
/// @param int_frame 
void _isr_handler(struct interrupt_frame* int_frame) {
    kprintf("RIP: %lx\n", int_frame->rip);
    k_bugcheck("A CPU exception has occured at !");
}