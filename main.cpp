#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <sys/time.h>

#include <string>

#include "led.h"
#include "cmd.h"

#define FIFO_FILE "/tmp/LED_FIFO"
#define BUFSIZE 512

Cmd *createCmd(){
    Cmd *cmd = new Cmd;
    int n = cmd->addCommand("set-led-state", Led::setState, true);
    cmd->addCommandArg(n, "off", 0);
    cmd->addCommandArg(n, "on", 1);
    n = cmd->addCommand("get-led-state", Led::getState, false);
    cmd->addCommandArg(n, "off", 0);
    cmd->addCommandArg(n, "on", 1);
    n = cmd->addCommand("set-led-color", Led::setColor, true);
    cmd->addCommandArg(n, "red", RED);
    cmd->addCommandArg(n, "green", GREEN);
    cmd->addCommandArg(n, "blue", BLUE);
    n = cmd->addCommand("get-led-color", Led::getColor, false);
    cmd->addCommandArg(n, "red", RED);
    cmd->addCommandArg(n, "green", GREEN);
    cmd->addCommandArg(n, "blue", BLUE);
    n = cmd->addCommand("set-led-rate", Led::setRate, true);
    cmd->addCommandArg(n, "0", 0);
    cmd->addCommandArg(n, "1", 1);
    cmd->addCommandArg(n, "2", 2);
    cmd->addCommandArg(n, "3", 3);
    cmd->addCommandArg(n, "4", 4);
    cmd->addCommandArg(n, "5", 5);
    n = cmd->addCommand("get-led-rate", Led::getRate, false);
    cmd->addCommandArg(n, "0", 0);
    cmd->addCommandArg(n, "1", 1);
    cmd->addCommandArg(n, "2", 2);
    cmd->addCommandArg(n, "3", 3);
    cmd->addCommandArg(n, "4", 4);
    cmd->addCommandArg(n, "5", 5);
    return cmd;
}

void *threadFn(void *arg){
    int fd;
    struct pollfd fds;
    char buf[BUFSIZE];
    char fifo[BUFSIZE];
    struct stat st;
    void **arr = (void**)arg;
    char *target = (char*)arr[0];
    int polr = 0;
    bzero(fifo, BUFSIZE);
    strcpy(fifo, target);
    Cmd *cmd = (Cmd*)arr[1];

    fd = open(fifo, O_RDONLY);
    if (!fd) {
	fprintf(stderr, "Can not open %s\n", fifo);
	return NULL;
    }
    fds.fd = fd;
    fds.events = POLLIN;
    polr = poll(&fds, 1, 1000);
    if (polr < 0) {
	fprintf(stderr, "Poll error for %s\n", fifo);
	return NULL;
    }
    if (!polr) {
	fprintf(stderr, "Can not read from %s by timeout\n", fifo);
	if (!stat(fifo, &st)) unlink(fifo);
	return NULL;
    }

    FILE *fp = fdopen(fd, "r");
    if (!fp) {
	fprintf(stderr, "Can not read %s\n", fifo);
	return NULL;
    }
    fgets(buf, BUFSIZE, fp);
    fclose(fp);

    if (buf[BUFSIZE - 1]) {
	fprintf(stderr, "Recieved string is too long, exit\n");
	return NULL;
    }
    cmd->exec(buf, BUFSIZE);
    fp = fopen(fifo, "w");
    if (!fp) {
	fprintf(stderr, "Can not write to %s\n", fifo);
	return NULL;
    }
    fwrite(buf, 1, strlen(buf), fp);
    fclose(fp);
    sleep(1);
    if (!stat(fifo, &st)) unlink(fifo);
    return NULL;
}

void sigHandler(int s){
    unlink(FIFO_FILE);
    _exit(0);
}

int main(int argc, char **argv){
    char buf[BUFSIZE];
    FILE *fp;
    Led led;
    Cmd *cmd;
    struct sigaction new_action;

    if ((argc > 1) && !strcmp(argv[1], "-d")) {
	pid_t pid = fork();
	if (pid < 0) {
	    fprintf(stderr, "Can not fork\n");
	    return -1;
	}
	if (pid) return 0;
    }

    new_action.sa_handler = sigHandler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGINT, &new_action, NULL);
    sigaction (SIGHUP, &new_action, NULL);
    sigaction (SIGTERM, &new_action, NULL);

    cmd = createCmd();

    umask(0);
    mknod(FIFO_FILE, S_IFIFO|0666, 0);
    while (1) {
	struct stat st;
	void *arr[2];
	fp = fopen(FIFO_FILE, "r");
	fgets(buf, BUFSIZE, fp);
	fclose(fp);
	if (buf[BUFSIZE - 1]) {
	    fprintf(stderr, "Recieved string is too long, continue\n");
	    continue;
	}
	buf[strlen(buf) - 1] = 0;
	if (!stat(buf, &st) && S_ISFIFO(st.st_mode)) {
	    pthread_t th;
	    arr[0] = buf;
	    arr[1] = cmd;
	    pthread_create(&th, NULL, threadFn, arr);
	    pthread_detach(th);
	}
    }
    return 0;
}
