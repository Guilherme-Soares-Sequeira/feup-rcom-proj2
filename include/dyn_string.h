#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Defines a struct that implements a dynamic length array of characters.
 * 5 Additional functions were implemented that should be used in conjunction
 * with this struct.
 *
 */
typedef struct dyn_string {
  /**
   * @brief Char pointer. If only the helper functions included in this file are
   * used, it is guaranteed to have at least one string ending character. Its
   * allocated size may not represent the length of the string.
   *
   */
  char *string;

  /**
   * @brief The number of characters that the string contains. Is equal to
   * strlen(dyn_string.string).
   *
   */
  size_t used;

  /**
   * @brief The number of characters that have been allocated to
   * dyn_string.string
   *
   */
  size_t buf_len;
} dyn_string;

/**
 * @brief Function used to instantiate a new dyn_string. The default is starting
 * with 0 characters.
 *
 * @return dyn_string* a pointer to the newly created dyn_string. Should be
 * freed (destroyed) when no longer necessary.
 */
dyn_string *dyn_string_new();

/**
 * @brief Function used to increase the length of the dynamic string. Handles
 * reallocating memory and updating the buf len variable.
 *
 * @param str The dynamic string that will have its length increased.
 * @param min_new The minimum number of characters that we want to add. For
 * efficiency purposes, more charactters may be allocated.
 */
void dyn_string_increase_len(dyn_string *const str, const size_t min_new);

/**
 * @brief Function that handles freeing a dynamic string. Handles freeing its
 * members and the dyn_string struct itself.
 *
 * @param to_destroy The dynamic string that will be destroyed.
 */
void dyn_string_destroy(dyn_string *const to_destroy);

/**
 * @brief Function that handles appending a character to the dynamic string.
 * Handles resizing if needed.
 *
 * @param pushee Dynamic string that to which the character should be apppended.
 * @param to_push Character that will be appended to the dynamic string.
 */
void dyn_string_push_chr(dyn_string *const pushee, const char to_push);

/**
 * @brief Function that handles appending a string to the dynamic string.
 * Handles resizing if needed.
 *
 * @param pushee Dynamic string that to which the new string should be
 * apppended.
 * @param to_push String that will be appended to the dynamic string.
 */
void dyn_string_push_str(dyn_string *const pushee, const char *const to_push);
