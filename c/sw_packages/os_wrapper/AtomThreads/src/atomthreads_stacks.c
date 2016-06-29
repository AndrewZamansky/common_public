/*
 * stacks.c
 *
 *  Created on: May 20, 2015
 *      Author: azamansk
 */

#include "_project.h"

uint8_t stack_storage_area[ATOMTHREADS_CONFIG_STACK_STORAGE_SIZE];

///* Main thread's stack area (large so place outside of the small page0 area on STM8) */
//uint8_t main_thread_stack[MAIN_STACK_SIZE_BYTES];
//
///* Idle thread's stack area (large so place outside of the small page0 area on STM8) */
//uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];
//
//
//
//uint8_t uart_thread_stack[UART_THREAD_STACK_SIZE];
//
//
//
//os_stack_info_t global_stacks_info[ATOMTHREADS_CONFIG_MAX_NUM_OF_TASKS] = {
//		{ MAIN_STACK_SIZE_BYTES , main_thread_stack},
////		{ IDLE_STACK_SIZE_BYTES , idle_thread_stack},
//		{ UART_THREAD_STACK_SIZE , uart_thread_stack}
//};
//
