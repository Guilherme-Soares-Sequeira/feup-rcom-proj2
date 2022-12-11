#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief Rough sketch of a struct that implements a dynamic length array of
 * strings. No support functions are provided.
 *
 */
typedef struct array {
  char **buf;
  size_t len;
  size_t used;
} array;

/**
 * @brief Struct that holds the fields required to login in FTP. If used outside
 * the scope of the functions provided in this file, care should be taken to
 * allocate and free its members.
 *
 */
typedef struct login_credentials {
  /**
   * @brief Char pointer that will contain the username used to login to the ftp
   * server.
   *
   */
  char *username;

  /**
   * @brief Char pointer that will contain the password used to login to the ftp
   * server.
   *
   */
  char *password;
} login_credentials;

/**
 * @brief Struct that contains information about the ftp server and the file we
 * are trying to access. If used outside the scope of the functions provided in
 * this file, care should be taken to allocate and free its members.
 *
 */
typedef struct ftp_info {
  /**
   * @brief Char pointer that will contain the full url that is passed to the
   * application.
   *
   */
  char *url;
  /**
   * @brief Char pointer that will contain the name of the host we are trying to
   * access. Is parsed from the url.
   *
   */
  char *host_name;
  /**
   * @brief Char pointer that will contain the path of the file we are trying to
   * access. Is parsed from the url.
   *
   */
  char *filepath;
  /**
   * @brief Char pointer that will contain the name of the file that will be
   * downloaded. It's also the name used to save the file.
   *
   */
  char *filename;
  /**
   * @brief login_credentials struct. Is initially parsed from the url. If not
   * provided, a default value is used. If it is rejected, a prompt will be
   * shown to the user and its values will be updated.
   *
   */
  login_credentials login_c;
} ftp_info;

/**
 * @brief Function that handles destroying (freeing) a ftp_info struct and all
 * of its members.
 *
 * @param to_destroy A pointer to the ftp_info struct to be destroyed.
 */
void ftp_info_destroy(ftp_info *to_destroy);

/**
 * @brief Compares two given strings, starting from their first character, up to
 * their num_chars'th character. Is case sensitive.
 *
 * @param str1 First of the strings to be compared.
 * @param str2 Second of the strings to be compared.
 * @param num_chars Number of characters that should be compared.
 * @return int 1 if the strings are equal up to num_chars characters, 0 if not
 * and -1 if an error occurred.
 */
int compare_strings(const char *const str1, const char *const str2,
                    const size_t num_chars);

/**
 * @brief Compares two given strings, starting from their ini_char'th character,
 * up to their end_char'th character. Is case sensitive.
 *
 * @param str1 First of the strings to be compared.
 * @param str2 Second of the strings to be compared.
 * @param ini_char Index of the initial character that will be compared.
 * @param end_char Index of the last character + 1. The character corresponding
 * to this index is NOT compared.
 * @return int 1 if the strings are equal in the specified range, 0 if not and
 * -1 if an error occurred.
 */
int compare_strings2(const char *const str1, const char *const str2,
                     const size_t ini_char, const size_t end_char);

/**
 * @brief Function that reads a line from the socket. Expects a string
 * terminated by a newline character.
 *
 * @param sockfd The file descriptor of the socket to be read.
 * @return char* Char pointer that contains the line read. Includes newline
 * character AND string ending character.
 */
char *read_line(const int sockfd);

/**
 * @brief Function that checks whether the given string indicates a multi line
 * answer from the FTP standard.
 *
 * @param str Char pointer that will be checked. Should be at least 4 characters
 * long.
 * @return int 1 if is, 0 if not or if the string has less than 4 characters.
 */
int is_multi_line(const char *const str);

/**
 * @brief Function that checks whether the given string is the last of the
 * answer or not, according to the FTP standard.
 *
 * @param str Char pointer that will be checked. Should be at least 4 characters
 * long.
 * @param code_num Char pointer that has the code which the checked string
 * should match. Should be at least 3 characters long.
 * @return int 1 if it is, 0 if not and -1 if the given string is not at least 4
 * characters long. Exits in case of failure.
 */
int is_final_line(const char *const str, const char *const code_num);

/**
 * @brief Function that reads an FTP answer from the specified socket, line by
 * line, and prints it.
 *
 * @param sockfd The file descriptor of the socket to be read.
 */
void read_print_ans(const int sockfd);

/**
 * @brief Function that reads an FTP answer from the specified socket, line by
 * line, and returns it in an array struct.
 *
 * @param sockfd The file descriptor of the socket to be read.
 * @return array* Array pointer to the newly created array struct. Care should
 * be taken to free it.
 */
array *read_ans(const int sockfd);

/**
 * @brief Function that prints an answer, line by line, from a previously read
 * answer contained in an array struct.
 *
 * @param ans Array pointer to the struct that was used to read the answer fromt
 * he socket.
 */
void print_ans(const array *const ans);

/**
 * @brief Function that parses the given URL to extract the provided username
 * and password. If none of these are provided, default values are returned
 * instead.
 *
 * @param url Char pointer to the URL that will be parsed.
 * @return login_credentials struct that contains the parsed username and
 * password.
 */
login_credentials get_login(const char *const url);

/**
 * @brief Function used to parse the URL and store valuable information inside a
 * ftp_info struct.
 *
 * @param url Char pointer to the url to be parsed.
 * @return ftp_info struct that contains valuable information, such as login
 * credentials, parsed from the given url.
 */
ftp_info parse_url(char *const url);

/**
 * @brief Function that initiates a socket and connects to the given host, at
 * the provided port. If the hostname is invalid, an error messaged is printed
 * and the program is terminated.
 *
 * @param sockfd Int pointer that will contain the file descriptor of the newly
 * opened socket.
 * @param hostname Char pointer to the name of the host that we are trying to
 * connect to.
 * @param port Unsigned 16 bit Int that contains the port used to connect to the
 * host.
 */
void initiate_sock(int *const sockfd, char *hostname, uint16_t port);

/**
 * @brief Function that initiates a socket and connects to the given host, using
 * the information provided in the sockaddr_in struct..
 *
 * @param sockfd Int pointer that will contain the file descriptor of the newly
 * opened socket.
 * @param s_addr sockaddr_in struct that contains the information used to
 * connect to the desired host.
 */
void initiate_sock_addrin(int *const sockfd,
                          const struct sockaddr_in *const s_addr);

/**
 * @brief Function that closes a socket.
 *
 * @param sockfd File descriptor of the socket to be closed.
 */
void close_sock(const int sockfd);

/**
 * @brief Function that logs in to an FTP server. If the provided login
 * credentials are not accepted, the user is prompted to input new ones until
 * they are accepted.
 *
 * @param sockfd File descriptor of the socket corresponding to the FTP server
 * we want to login to.
 * @param login_c login_credentials struct that contains the initial information
 * used to login. If these are not accepted, the members of this struct will be
 * updated with new ones.
 */
void login(const int sockfd, login_credentials *const login_c);

/**
 * @brief Function that sends a request to enter passive mode to an FTP server,
 * parses its content and return a sockaddr_in containing information such as
 * the ip and port we should use to receive the files from.
 *
 * @param sockfd File descriptor of the socket corresponding to the FTP server
 * we are trying to communicate with.
 * @return struct sockaddr_in* a pointer to the newly allocated struct with the
 * information to open the socket that should be used to receive data from the
 * server.
 */
struct sockaddr_in *enter_psv(const int sockfd);

/**
 * @brief Requests a file from the FTP server.
 *
 * @param sockfd File descriptor of the socket corresponding to the FTP server
 * we want to request a file from.
 * @param filepath The path of the file that will be requested.
 */
void request_file(int sockfd, const char *const filepath);

/**
 * @brief Receives a file from a socket and writes it to a file in the CWD.
 *
 * @param sockfd File descriptor corresponding to the FTP server from which the
 * data will be received. In the scope of this project, it will correspond to
 * the address received when we enter passive mode.
 * @param filename The name the file should be saved with.
 */
void write_file(int sockfd, const char *const filename);