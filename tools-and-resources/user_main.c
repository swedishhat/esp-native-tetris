/*
This is where you specify libraries from the SDK to perform certain tasks.
Means the same thing as #include in Arduino sketches. All the system header
files can be found in [install directory]/esp-open-sdk/sdk/include/
*/
#include "ets_sys.h"        // Event signals and task priorities
#include "osapi.h"          // General ESP functions (timers, strings, memory)
#include "gpio.h"           // Interacting and configuring pins
#include "os_type.h"        // Mapping to ETS structures
#include "user_config.h"    // Any user-defined functions. Alway required.

/*
Define is a way to save program space when using constants. This basically
tells the compiler that anywhere the terms user_procTaskPrio or
user_procTaskQueueLen show up, replace it with a 0 and a 1 respectively.
*/
//#define user_procTaskPrio        0
//#define user_procTaskQueueLen    1

/*
Creates a message queue to ask the ESP to perform a user-defined task
like waiting and blinking.
From https://en.wikipedia.org/wiki/Message_queue:
Message queues provide an asynchronous communications protocol between
processes, meaning that the sender and receiver of the message do not
need to interact with the message queue at the same time.Messages placed
onto the queue are stored until the recipient retrieves them.
*/
//os_event_t user_procTaskQueue[user_procTaskQueueLen];


/*
STATIC VARIABLE:
    When used outside of any functions, at the top-level of a file, static
    limits the scope to just the file in question
    http://hackaday.com/2015/08/04/embed-with-elliot-the-static-keyword-you-dont-fully-understand/
*/

/*
VOLATILE VARIABLE:
    If you want to define a file-scoped, persistant variable that can change 
    without warning (and this is common), you’d want static volatile.
    http://hackaday.com/2015/08/18/embed-with-elliot-the-volatile-keyword/
*/


// Function prototype for the loop function with file-wide scope
//static void user_procTask(os_event_t *events);

// Don't optimize away anything on the timer type and let it be
// file-wide in scope
static volatile os_timer_t some_timer;


// Do the blinky stuff
void some_timerfunc(void *arg)
{
    // If GPIO_PIN2 is on, turn it off and vice versa
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2)
    {
        //Set GPIO2 to LOW
        gpio_output_set(0, BIT2, BIT2, 0);
    }
    else
    {
        //Set GPIO2 to HIGH
        gpio_output_set(BIT2, 0, BIT2, 0);
    }
}

/*
Do nothing function. This is the loop() equivalent in Arduino. We could put
the blink code in here and use delays but this would "block" anything else
from happening while we were waiting for the delay function to finish.
Hella inefficient and not good programming. The actual blinking happens in the
timer interrupt routine some_timerfunc() but we need to keep 

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{
    os_delay_us(10);
}
*/

/*
Init function which is like the setup() function in Arduino. ICACHE_FLASH_ATTR
tells the linker that the function should be stored in flash memory instead of
RAM. It is generally advisable to prefix most functions with ICACHE_FLASH_ATTR
in order to save RAM.
http://www.penninkhof.com/2015/03/how-to-program-an-esp8266/
*/
void ICACHE_FLASH_ATTR
user_init()
{
    // Initialize the GPIO subsystem.
    gpio_init();

    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

    //Set GPIO2 low
    gpio_output_set(0, BIT2, BIT2, 0);

    //Disarm timer
    os_timer_disarm(&some_timer);

    //Setup timer
    os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    os_timer_arm(&some_timer, 1000, 1);
    
    /*
    system_os_task: Start os task of doing nothing. 
        -- user_procTask: the task function to be called, in this case, the 
           "do nothing" function.
        -- user_procTaskPrio: task priority. 3 priorities are supported: 0-2,
           where 0 is the lowest. This means only 3 tasks is allowed to set up.
        -- user_procTaskQueue: pointer to the message queue
        -- user_procTaskQueueLen: length of the queue
    */
    //system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
}