/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

#include <arpa/inet.h>
#include <fcntl.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 21

typedef struct login_credentials {
  char username[32];
  char password[32];
} login_credentials;

typedef struct ftp_info {
  char url[128];
  char host_name[32];
  char filepath[64];
  char filename[32];
  login_credentials login_c;
} ftp_info;

typedef struct dyn_string {
  char *string;
  size_t used, buf_len;
} dyn_string;

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

int compare_strings(const char *const str1, const char *const str2,
                    const size_t num_chars) {
  if (strlen(str1) < num_chars || strlen(str2) < num_chars) {
    printf("[ERROR] Tried to compare strings with an invalid length. str1.len "
           "= %lu, "
           "str2.len = %lu, num_chars = %lu\n",
           strlen(str1), strlen(str2), num_chars);
    return -1;
  }
  for (size_t i = 0; i < num_chars; i++) {
    if (str1[i] != str2[i])
      return 0;
  }
  return 1;
}

int compare_strings2(const char *const str1, const char *const str2,
                     const size_t ini_char, const size_t end_char) {
  if (strlen(str1) < ini_char || strlen(str1) < end_char ||
      end_char - ini_char > strlen(str2) || end_char < ini_char) {
    printf("[ERROR] Tried to compare strings with an invalid length. str1.len "
           "= %lu, "
           "str2.len = %lu, ini_char = %lu, end_char = %lu\n",
           strlen(str1), strlen(str2), ini_char, end_char);
    return -1;
  }
  for (size_t i = ini_char, j = 0; i < end_char; i++) {
    if (str1[i] != str2[j])
      return 0;
    j++;
  }
  return 1;
}

char *read_line(const int sockfd) {
  dyn_string *str = dyn_string_new();

  char read_char;
  read(sockfd, &read_char, 1);
  dyn_string_push_chr(str, read_char);

  while (read_char != '\n') {
    read(sockfd, &read_char, 1);
    dyn_string_push_chr(str, read_char);
  }

  char *ret = strdup(str->string);
  dyn_string_destroy(str);

  return ret;
}

int is_multi_line(const char *const str) {
  if (strlen(str) < 4) {
    return 0;
  }
  if (str[3] == '-')
    return 1;
  return 0;
}

int is_final_line(const char *const str, const char *const code_num) {
  if (strlen(str) < 4)
    return -1;

  int comp_result;
  if ((comp_result = compare_strings(str, code_num, 3)) == -1) {
    printf("[ERROR] Critical error: tried to compare\n %s\n and\n %s\n when "
           "the length "
           "of one of the strings isn't at least 3.\n",
           str, code_num);
    exit(-1);
  }
  return comp_result && str[3] == ' ';
}

void read_print_ans(const int sockfd) {
  sleep(1);

  char *first_line = read_line(sockfd);

  if (!is_multi_line(first_line)) {
    printf("[SYS] Ans:\n\n%s\n\n/ans\n", first_line);
    free(first_line);
    return;
  }

  char *line;

  // create dynamic string and append first line
  dyn_string *ans = dyn_string_new();
  dyn_string_push_str(ans, first_line);

  // read while we don't get the last line
  while (!is_final_line(line = read_line(sockfd), first_line)) {
    dyn_string_push_str(ans, line);
    free(line);
  }
  dyn_string_push_str(ans, line);
  free(line);

  // print answer, free pointers and return
  printf("[SYS] Ans:\n\n%s\n\n/ans\n\n", ans->string);
  dyn_string_destroy(ans);
  free(first_line);
}

login_credentials get_login(const char *const url) {
  char *url_copy = strdup(url);
  login_credentials ret;
  bzero(ret.username, sizeof(ret.username));
  bzero(ret.password, sizeof(ret.password));
  if (strchr(url_copy, '@') == NULL) {
    strcpy(ret.username, "anonymous");
    strcpy(ret.password, "qualquer-pass");
    return ret;
  }

  char *username, *pass;
  if ((username = strstr(url_copy, "ftp://")) != NULL) {
    username += 6;
  }

  char delim[2] = ":";
  username = strtok(username, delim);
  delim[0] = '@';
  pass = strtok(NULL, delim);
  strcpy(ret.username, username);
  strcpy(ret.password, pass);
  free(url_copy);
  return ret;
}

ftp_info parse_url(char *const url) {
  ftp_info ret;
  ret.login_c = get_login(url);
  strcpy(ret.url, url);

  char *hostname, *url_copy;
  url_copy = strdup(url);
  if ((hostname = strchr(url_copy, '@')) == NULL) {
    if ((hostname = strstr(url_copy, "ftp://")) ==
        NULL) { // there is no ftp:// and no @, host name is the first thing in
                // the url
      hostname = url_copy;
    } else { // there is ftp:// but no @, adjust pointer to the beginning of the
             // host name
      hostname += 6;
    }
  } else { // character after @
    hostname += 1;
  }
  char delim[2] = "/";
  hostname = strtok(hostname, delim);
  strcpy(ret.host_name, hostname);
  strcpy(ret.filepath, strstr(url, hostname) + strlen(hostname) + 1);

  char *filename, *temp = url;
  while ((temp = strchr(temp, '/')) != NULL) {
    temp += 1;
    filename = temp;
  }
  strcpy(ret.filename, filename);

  free(url_copy);
  return ret;
}

void initiate_sock(int *const sockfd, char *hostname, uint16_t port) {
  struct sockaddr_in s_addr;

  struct hostent *h;

  if ((h = gethostbyname(hostname)) == NULL) {
    herror("[ERROR] Gethostbyname()");
    exit(-1);
  }

  char *server_ip = inet_ntoa(*((struct in_addr *)h->h_addr));

  /*server address handling*/
  bzero((char *)&s_addr, sizeof(s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_addr.s_addr =
      inet_addr(server_ip); /*32 bit Internet address network byte ordered*/
  s_addr.sin_port =
      htons(port); /*server TCP port must be network byte ordered */

  /*open a TCP socket*/
  if (((*sockfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("[ERROR] Socket()");
    exit(-1);
  }
  printf("[SYS] Opened TCP Socket.\n");
  /*connect to the server*/
  if (connect(*sockfd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
    perror("[ERROR] connect()");
    exit(-1);
  }
  printf("[SYS] Connected.\n");
  read_print_ans(*sockfd);
}

void initiate_sock_addrin(int *const sockfd,
                          const struct sockaddr_in *const s_addr) {
  /*open a TCP socket*/
  if (((*sockfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("[ERROR] Socket()");
    exit(-1);
  }
  printf("[SYS] Opened TCP Socket.\n");
  /*connect to the server*/
  if (connect(*sockfd, (struct sockaddr *)s_addr, sizeof(struct sockaddr_in)) <
      0) {
    perror("[ERROR] Connect()");
    exit(-1);
  }
  printf("[SYS] Connected.\n");
}

void close_sock(const int sockfd) {
  if (close(sockfd) < 0) {
    perror("[ERROR] Close()");
    exit(-1);
  }
}

void login(const int sockfd, const char *const username,
           const char *const password) {
  char user_cmd[36] = "user ";
  char *send_user = strcat(strcat(user_cmd, username), "\n");
  printf("[SYS] Send_user = %s...\n", send_user);
  write(sockfd, send_user, strlen(send_user));
  read_print_ans(sockfd);
  char pass_cmd[36] = "pass ";
  char *send_pass = strcat(strcat(pass_cmd, password), "\n");
  write(sockfd, send_pass, strlen(send_pass));
  read_print_ans(sockfd);
}

struct sockaddr_in *enter_psv(const int sockfd) {
  char *pasv = "pasv\n";
  write(sockfd, pasv, strlen(pasv));
  sleep(1);
  char *ans = read_line(sockfd);

  char IP[16] = {0};
  size_t ans_index = 27, IP_index = 0, comma_counter = 0;

  // extract IP from the ans
  while (1) {
    if (ans[ans_index] == ',') {
      if (comma_counter == 3) {
        ans_index++; // set ans_index to first element of IP
        break;
      }
      comma_counter++;
      IP[IP_index] = '.';
      IP_index++;
      ans_index++;
      continue;
    } else {
      IP[IP_index] = ans[ans_index];
      IP_index++;
      ans_index++;
    }
  }

  // extract port from ans

  char byte1[4] = {0}, byte2[4] = {0};
  int got_first_byte = 0;
  size_t byte1_index = 0, byte2_index = 0;

  while (1) {
    if (ans[ans_index] == ')') {
      break;
    } else if (ans[ans_index] == ',') {
      got_first_byte = 1;
      ans_index++;
    } else if (got_first_byte == 0) {
      byte1[byte1_index] = ans[ans_index];
      byte1_index++;
      ans_index++;
    } else if (got_first_byte) {
      byte2[byte2_index] = ans[ans_index];
      byte2_index++;
      ans_index++;
    } else {
      perror("[ERROR] Error while getting the port number.\n");
    }
  }
  int port = atoi(byte1) * 256 + atoi(byte2);

  printf("[SYS] Ans = %s\n", ans);
  printf("[SYS] IP = %s\n", IP);
  printf("[SYS] Port = %s,%s or %i\n", byte1, byte2, port);

  printf("[SYS] Size of sockaddr_in = %lu\n", sizeof(struct sockaddr_in));
  struct sockaddr_in *ret =
      (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
  bzero((char *)ret, sizeof(struct sockaddr_in));
  ret->sin_family = AF_INET;
  ret->sin_addr.s_addr = inet_addr(IP);
  ret->sin_port = htons(port);
  free(ans);
  return ret;
}

void request_file(int sockfd, const char *const filepath) {
  char retr_cmd[36] = "retr ";
  char *send_retr = strcat(strcat(retr_cmd, filepath), "\n");
  write(sockfd, send_retr, strlen(send_retr));
  read_print_ans(sockfd);
}

void write_file(int sockfd, const char *const filename) {
  int fd = open(filename, O_RDWR | O_CREAT);
  char bytestream[1024] = {0};
  int num_bytes;
  while ((num_bytes = read(sockfd, bytestream, sizeof(bytestream)))) {
    write(fd, bytestream, num_bytes);
  }
  close(fd);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr,
            "[ERROR] Usage: %s <address to the file to be downloaded>\n",
            argv[0]);
    exit(-1);
  }

  ftp_info info = parse_url(argv[1]);
  printf("[SYS] url = %s\nhostname = %s\n username = %s\n password = %s\n "
         "filepath = "
         "%s\n filename = %s\n",
         info.url, info.host_name, info.login_c.username, info.login_c.password,
         info.filepath, info.filename);

  int control_sockfd;
  initiate_sock(&control_sockfd, info.host_name, 21);

  login(control_sockfd, info.login_c.username, info.login_c.password);
  struct sockaddr_in *data_socket = enter_psv(control_sockfd);
  int data_sockfd;
  initiate_sock_addrin(&data_sockfd, data_socket);

  request_file(control_sockfd, info.filepath);
  write_file(data_sockfd, info.filename);

  close_sock(control_sockfd);
  close_sock(data_sockfd);
  return 0;
}
