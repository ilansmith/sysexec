#ifndef _SYSEXEC_H_ 
#define _SYSEXEC_H_

#include <stdarg.h>

/** Execute a command
 * @param fmt    format string specifying how subsequent arguments are parsed
 * 
 * @return 0 on success, -1 otherwise
 */
int sysexec(char *fmt, ...);

/** Execute a command with a user environment
 * @param env    an array of pointers to null-terminated strings of the form:
 *               name=value. Must be terminated by a NULL pointer
 * @param fmt    format string specifying how subsequent arguments are parsed
 * 
 * @return 0 on success, -1 otherwise
 */
int sysexece(char **env, char *fmt, ...);

/** Execute a command and retrieve its output in the parent process
 * @param buf    a buffer from which the output of the child process' is
 *               placed
 * @param count  the length of the buf
 * @param fmt    format string specifying how subsequent arguments are parsed
 * 
 * @return 0 on success, -1 otherwise
 */
int sysexecp(char *buf, int count, char *fmt, ...);

/** Execute a command and retrieve its output in the parent process
 * @param buf    a buffer from which the output of the child process' is
 *               placed
 * @param count  the length of the buf
 * @param env    an array of pointers to null-terminated strings of the form:
 *               name=value. Must be terminated by a NULL pointer
 * @param fmt    format string specifying how subsequent arguments are parsed
 * 
 * @return 0 on success, -1 otherwise
 */
int sysexecpe(char *buf, int count, char **env, char *fmt, ...);

#endif

