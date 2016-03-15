/*
 * Copyright (C)  2015 Balint Buchert
 * balintbuchert@gmail.com
 *
 * Firefly Clean Energy -- Climate Master Projcet
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * File function:
 * -PWM initial
 * -Base port inputs outputs initial
 *
 */
/** \file climate_conf
    \brief climate_conf
*/

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "shell.h"

/*
 * PWM configuration structure.
 * Cyclic callback enabled, channels 1 and 4 enabled without callbacks,
 * the active state is a logic one.
 */
static const PWMConfig pwmcfg = {
  1300000,                                   /* 100kHz PWM clock frequency.  */
  256,                                       /* PWM period is 128 cycles.    */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  /* HW dependent part.*/
  0,
  0
};

static const PWMConfig pwmcfg2 = {
  1300000,                                   /* 100kHz PWM clock frequency.  */
  256,                                       /* PWM period is 128 cycles.    */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  /* HW dependent part.*/
  0,
  0
};



void baseInit(void){
  /*
   * Activates the serial driver 2 using the driver default configuration.
   * PA2(TX) and PA3(RX) are routed to USART2.
   */
  //sdStart(&SD2, NULL);
  //palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  //palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOC, 0, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, 1, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, 2, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, 3, PAL_MODE_INPUT);
    palSetPadMode(GPIOE, 7, PAL_MODE_OUTPUT_PUSHPULL);

  /*
   * Initializes the PWM driver 4, routes the TIM4 outputs to the board LEDs.
   */

    palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_ALTERNATE(2));      /* Green.   */ //0
    palSetPadMode(GPIOD, GPIOD_LED3, PAL_MODE_ALTERNATE(2));      /* Orange.  *///1/
    palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_ALTERNATE(2));      /* Red.     *///2
  //palSetPadMode(GPIOE, 1, PAL_MODE_ALTERNATE(2));      /* Red.     */
 // palSetPadMode(GPIOE, GPIOD_LED6, PAL_MODE_ALTERNATE(2));      /* Blue.    */
  //palSetPadMode(GPIOE,11, PAL_MODE_ALTERNATE(1));      /* Blue.    */
   palSetPadMode(GPIOE, GPIOE_PIN9, PAL_MODE_ALTERNATE(1));

       //palSetPad(GPIOE, 8);
    pwmStart(&PWMD4, &pwmcfg);
    pwmStart(&PWMD1, &pwmcfg2);

}
