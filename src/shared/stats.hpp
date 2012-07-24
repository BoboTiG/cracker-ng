
/*!
 * \file stats.hpp
 * \brief Statistics headers.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.03.04
 */


#define SLEEPING_TIME 5 //!< Time to wait between each statistic (in secondes)
#define NUMBER_SEP ","  //!< Number separator


// Prototypes
/*!
 * \fn time_t current_time()
 * \brief Get the local time using a sub second resolution clock.
 * \return Time in microseconds.
 */
static time_t current_time();

/*!
 * \fn elapsed_seconds(const time_t & start_time)
 * \brief Calcul to time elapsed between to moments.
 * \param Time in seconds.
 */
static unsigned long elapsed_seconds(const time_t & start_time);

/*!
 * \fn format_number(const unsigned long & num)
 * \brief Format a number.
 * Into stats.hpp you can define the separator, default is ",".
 * \param num Number to format.
 */
static string format_number(const unsigned long & num);

/*!
 * \fn stats(void* argz)
 * \brief Print statistics (passwords/sec).
 * \param argz Statistics structure to pass arguments.
 */
void* stats(void* argz);

/*!
 * \fn stats_sumary(const unsigned long & num, const time_t & start_time)
 * \brief Print statistics sumary.
 * \param num Number of tried passwords.
 * \param start_time Cracking process start time.
 */
static void stats_sumary(const unsigned long & num, const time_t & start_time);

// Structures
/*!
 * \struct statistics
 * \brief stats routine arguments structure.
 */
struct statistics {
	unsigned int  * num;
	unsigned long * total;
	time_t        * start_time;
};
