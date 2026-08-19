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

.intel_syntax noprefix

.section .text
.code64
.global _halt_catch_fire

# Architecture specific implementation of the HCF function!
# Each architecure IS DIFFRENT!
_halt_catch_fire:
    cli
._hcf_loop:
    hlt
    jmp ._hcf_loop