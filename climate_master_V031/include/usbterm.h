#ifndef USBTERM_H_INCLUDED
#define USBTERM_H_INCLUDED
/*
 * Copyright (C)  2015 Balint Buchert balintbuchert@gmail.com
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/** \file usbterm
    \brief usbterm
*/

#define usbterm_STACK_SIZE 128

#define usbterm_SLEEP_TIME_US 10000


/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

BaseSequentialStream *serial = (BaseSequentialStream*)&SDU1;


void usbtermInit(void);

void cmd_usbterm(BaseSequentialStream *chp, int argc, char *argv[]);

#endif // usbterm_H_INCLUDED
