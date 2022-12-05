/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

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

void read_print_ans(int sockfd) {
    sleep(1);
    char ans[1024] = {0};
    read(sockfd, ans, sizeof(ans));
    ans[1023] = 0;
    printf("ans: %s\n!!<<>>!!\n", ans);
}

login_credentials get_login(const char * const url) {
    char* url_copy = strdup(url);
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

ftp_info parse_url(char * const url) {
    ftp_info ret;
    ret.login_c = get_login(url);
    strcpy(ret.url, url);

    char* hostname, *url_copy;
    url_copy = strdup(url);
    if ((hostname = strchr(url_copy, '@')) == NULL) {
        if ((hostname = strstr(url_copy, "ftp://")) == NULL) { //there is no ftp:// and no @, host name is the first thing in the url
            hostname = url_copy;
        }
        else { //there is ftp:// but no @, adjust pointer to the beginning of the host name
            hostname += 6;
        }
    } else {  //character after @
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

int compare_strings(const char * const str1, const char * const str2, const size_t num_chars) {
    if (strlen(str1) > num_chars || strlen(str2) > num_chars) {
        printf("tried to compare strings with an invalid length. str1.len = %lu, str2.len = %lu, num_chars = %lu\n", strlen(str1), strlen(str2), num_chars);
        return -1;
    }
    for (size_t i = 0; i < num_chars; i++) {
        if (str1[i] != str2[i]) return 0;
    }
    return 1;
}

int compare_strings2(const char * const str1, const char * const str2, const size_t ini_char, const size_t end_char) {
    if (strlen(str1) > ini_char || strlen(str1) > end_char || end_char - ini_char > strlen(str2) || end_char > ini_char) {
        printf("tried to compare strings with an invalid length. str1.len = %lu, str2.len = %lu, ini_char = %lu, end_char = %lu\n", strlen(str1), strlen(str2), ini_char, end_char);
        return -1;
    }
    for (size_t i = ini_char, j = 0; i < end_char; i++) {
        if (str1[i] != str2[j]) return 0;
        j++;
    }
    return 1;

}

void initiate_sock(int * const sockfd, char* hostname, uint16_t port) {
    struct sockaddr_in s_addr;

    struct hostent *h;

    if ((h = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    char* server_ip = inet_ntoa(*((struct in_addr *) h->h_addr));

    /*server address handling*/
    bzero((char *) &s_addr, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(server_ip);    /*32 bit Internet address network byte ordered*/
    s_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if (((*sockfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    printf("Opened TCP Socket.\n");
    /*connect to the server*/
    if (connect(*sockfd,
                (struct sockaddr *) &s_addr,
                sizeof(s_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }
    printf("Connected.\n");
    read_print_ans(*sockfd);
}

void initiate_sock_addrin(int * const sockfd, const struct sockaddr_in* const s_addr) {
    /*open a TCP socket*/
    if (((*sockfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    printf("Opened TCP Socket.\n");
    /*connect to the server*/
    if (connect(*sockfd, (struct sockaddr *) s_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("connect()");
        exit(-1);
    }
    printf("Connected.\n");
}

void close_sock(const int sockfd) {
    if (close(sockfd) < 0) {
        perror("close()");
        exit(-1);
    }
}

void login(const int sockfd, const char * const username, const char * const password) {
    char user_cmd[36] = "user ";
    char* send_user = strcat(strcat(user_cmd, username), "\n");
    printf("send_user = %s...\n", send_user);
    write(sockfd, send_user, strlen(send_user));
    read_print_ans(sockfd);
    char pass_cmd[36] = "pass ";
    char* send_pass = strcat(strcat(pass_cmd, password), "\n");
    write(sockfd, send_pass, strlen(send_pass));
    read_print_ans(sockfd);
}

struct sockaddr_in* enter_psv(const int sockfd) {
    char* pasv = "pasv\n";
    write(sockfd, pasv, strlen(pasv));
    sleep(1);
    char ans[1024] = {0};
    read(sockfd, ans, sizeof(ans));

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
            IP_index++; ans_index++;
            continue;
        } else {
            IP[IP_index] = ans[ans_index];
            IP_index++; ans_index++;
        }
    }

    // extract port from ans

    char byte1[4] = {0}, byte2[4] = {0};
    int got_first_byte = 0;
    size_t byte1_index = 0, byte2_index = 0;

    while (1) {
        if (ans[ans_index] == ')') {
            break;
        }
        else if (ans[ans_index] == ',') {
            got_first_byte = 1;
            ans_index++;
        }
        else if (got_first_byte == 0) {
            byte1[byte1_index] = ans[ans_index];
            byte1_index++; ans_index++;
        }
        else if (got_first_byte) {
            byte2[byte2_index] = ans[ans_index];
            byte2_index++; ans_index++;
        }
        else {
            perror("Error while getting the port number.\n");
        }
    }
    int port = atoi(byte1) * 256 + atoi(byte2);

    printf("ans = %s\n", ans);
    printf("IP = %s\n", IP);
    printf("Port = %s,%s or %i\n", byte1, byte2, port);

    printf("size of sockaddr_in = %lu\n", sizeof(struct sockaddr_in));
    struct sockaddr_in* ret = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
    bzero((char *) ret, sizeof(struct sockaddr_in));
    ret->sin_family = AF_INET;
    ret->sin_addr.s_addr = inet_addr(IP);
    ret->sin_port = htons(port);
    return ret;
}

void request_file(int sockfd, const char * const filepath) {
    char retr_cmd[36] = "retr ";
    char* send_retr = strcat(strcat(retr_cmd, filepath), "\n");
    write(sockfd, send_retr, strlen(send_retr));
    read_print_ans(sockfd);
}

void write_file(int sockfd, const char * const filename) {
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
        fprintf(stderr, "Usage: %s <address to the file to be downloaded>\n", argv[0]);
        exit(-1);
    }

    ftp_info info = parse_url(argv[1]);
    printf("url = %s\nhostname = %s\n username = %s\n password = %s\n filepath = %s\n filename = %s\n", info.url, info.host_name, info.login_c.username, info.login_c.password, info.filepath, info.filename);
    
    int control_sockfd;
    initiate_sock(&control_sockfd, info.host_name, 21);

    login(control_sockfd, info.login_c.username, info.login_c.password);
    struct sockaddr_in* data_socket = enter_psv(control_sockfd);
    int data_sockfd;
    initiate_sock_addrin(&data_sockfd, data_socket);

    request_file(control_sockfd, info.filepath);
    write_file(data_sockfd, info.filename);

    close_sock(control_sockfd);
    close_sock(data_sockfd);
    return 0;
}


