#include "../include/dyn_string.h"

dyn_string *dyn_string_new() {
  char *initial_string = (char *)malloc(sizeof(char) * 1);
  *initial_string = '\0';
  dyn_string *ret = malloc(sizeof(dyn_string));
  ret->string = initial_string;
  ret->buf_len = 1;
  ret->used = 0;
  return ret;
}

void dyn_string_destroy(dyn_string *const to_destroy) {
  free(to_destroy->string);
  free(to_destroy);
}

void dyn_string_increase_len(dyn_string *const str, const size_t min_new) {
  size_t min_size = str->buf_len + min_new;
  size_t new_size = str->buf_len;
  while (new_size < min_size)
    new_size = ceil(new_size * 1.5) +
               1e-3; // float to int convertion requires a small value to be
                     // added in order to work correctly

  str->string = realloc(str->string, new_size);
  str->buf_len = new_size;
}

void dyn_string_push_chr(dyn_string *const pushee, const char to_push) {
  if (pushee->used + 1 == pushee->buf_len)
    dyn_string_increase_len(pushee, 1);

  pushee->string[pushee->used] = to_push;
  pushee->string[pushee->used + 1] = '\0';
  pushee->used++;
}

void dyn_string_push_str(dyn_string *const pushee, const char *const to_push) {
  if (pushee->used + strlen(to_push) >= pushee->buf_len)
    dyn_string_increase_len(pushee, strlen(to_push));

  strcpy(pushee->string + pushee->used, to_push);
  pushee->used += strlen(to_push);
}
