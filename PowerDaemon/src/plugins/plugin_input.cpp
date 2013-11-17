/*
 *  Copyright (C) 2002-2008  Mattia Dongili <malattia@linux.it>
 *                           George Staikos <staikos@0wned.org>
 *  Copyright (C) 2012 Marvell, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <sys/limits.h>
#include <sys/poll.h>
#include <linux/input.h>

extern "C"
{
#include "ppd_component.h"
#include "plugin_tags.h"
}

#define SIGCANCEL SIGUSR1
#define INPUT_DRIVER_PATH "/sys/class/input"
#define INPUT_DEVICE_PATH "/dev/input"
#define INPUTEVENT_EXPIRE_TIME 200

#define MAX_BUF_LENTH 32

//NOTICE: ONCE YOU HAVE CHANGED TOUCH DRIVER, THIS WOULD NOT BE WORKED.
//        Please add the related touch driver in Andorid.mk.
static char touch_sysfs_node[MAX_BUF_LENTH];
static char key_sysfs_node[MAX_BUF_LENTH];

static pthread_t event_thread;
static struct sigaction prev_action;
static struct pollfd *ufds;
static char **device_names;
static int nfds;

static int open_device(const char *device)
{
    int version;
    int fd;
    struct pollfd *new_ufds;
    char **new_device_names;
    char name[80];
    char location[80];
    char idstr[80];
    struct input_id id;

    fd = open(device, O_RDWR);
    if(fd < 0) {
        return -1;
    }

    if(ioctl(fd, EVIOCGVERSION, &version)) {
        return -1;
    }
    if(ioctl(fd, EVIOCGID, &id)) {
        return -1;
    }
    name[sizeof(name) - 1] = '\0';
    location[sizeof(location) - 1] = '\0';
    idstr[sizeof(idstr) - 1] = '\0';
    if(ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
        name[0] = '\0';
    }
    if(ioctl(fd, EVIOCGPHYS(sizeof(location) - 1), &location) < 1) {
        location[0] = '\0';
    }
    if(ioctl(fd, EVIOCGUNIQ(sizeof(idstr) - 1), &idstr) < 1) {
        idstr[0] = '\0';
    }

    new_ufds = (pollfd *)realloc(ufds, sizeof(ufds[0]) * (nfds + 1));
    if(new_ufds == NULL) {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    ufds = new_ufds;
    new_device_names = (char**)realloc(device_names, sizeof(device_names[0]) * (nfds + 1));
    if(new_device_names == NULL) {
        fprintf(stderr, "out of memory\n");
        return -1;
    }
    device_names = new_device_names;
    ufds[nfds].fd = fd;
    ufds[nfds].events = POLLIN;
    device_names[nfds] = strdup(device);
    nfds++;

    return 0;
}

int close_device(const char *device)
{
    int i;
    for(i = 1; i < nfds; i++) {
        if(strcmp(device_names[i], device) == 0) {
            int count = nfds - i - 1;
            free(device_names[i]);
            memmove(device_names + i, device_names + i + 1, sizeof(device_names[0]) * count);
            memmove(ufds + i, ufds + i + 1, sizeof(ufds[0]) * count);
            nfds--;
            return 0;
        }
    }
    return -1;
}

static int read_notify(const char *dirname, int nfd)
{
    int res;
    char devname[PATH_MAX];
    char *filename;
    char event_buf[512];
    int event_size;
    int event_pos = 0;
    struct inotify_event *event;

    res = read(nfd, event_buf, sizeof(event_buf));
    if(res < (int)sizeof(*event)) {
        if(errno == EINTR)
            return 0;
        fprintf(stderr, "could not get event, %s\n", strerror(errno));
        return 1;
    }

    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';

    while(res >= (int)sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        if(event->len) {
            strcpy(filename, event->name);
            if(event->mask & IN_CREATE) {
                open_device(devname);
            }
            else {
                close_device(devname);
            }
        }
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return 0;
}

static int scan_dir(const char *dirname)
{
    char devname[PATH_MAX];
    char *filename;
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    clog(LOG_NOTICE, "touch_sysfs_node %s--key_sysfs_node %s\n", touch_sysfs_node, key_sysfs_node);
    while((de = readdir(dir))) {
        if (strcmp(touch_sysfs_node, de->d_name) == 0 ||
                strcmp(key_sysfs_node, de->d_name) == 0) {
            strcpy(filename, de->d_name);
            open_device(devname);
        }
    }
    closedir(dir);
    return 0;
}

int access_sysfs_node(char* path, char* buf) {
    memset(buf, 0, MAX_BUF_LENTH);
    int fd = open(path, O_RDONLY);
    int len = -1;
    if (fd < 0) {
        clog(LOG_ERR, "Couldn`t open %s---%s\n", path, strerror(errno));
        return -1;
    }

    len = read(fd, buf, MAX_BUF_LENTH);
    if (len <= 0) {
        clog(LOG_ERR, "Empty %s---%s\n", path, strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

void init_input_driver_info() {
    char driver_path[PATH_MAX] = {0};
    char driver_name[MAX_BUF_LENTH] = {0};
    DIR *dir;
    struct dirent *de;
    dir = opendir(INPUT_DRIVER_PATH);
    if(dir == NULL) {
        clog(LOG_ERR, "Can`t open dir %s---%s\n", INPUT_DRIVER_PATH, strerror(errno));
        return;
    }

    int index = -1;
    while((de = readdir(dir))) {
        if (strncmp(de->d_name, "input", strlen("input")) != 0) {
            continue;
        }
        memset(driver_path, 0, PATH_MAX);
        snprintf(driver_path, PATH_MAX, "%s/%s/name", INPUT_DRIVER_PATH, de->d_name);
        access_sysfs_node(driver_path, driver_name);
        sscanf(de->d_name, "input%d", &index);
        if (strncmp(driver_name, TOUCH_DRIVER, strlen(TOUCH_DRIVER)) == 0) {
            snprintf(touch_sysfs_node, MAX_BUF_LENTH, "event%d", index);
        } else if (strncmp(driver_name, KEY_DRVIER, strlen(KEY_DRVIER)) == 0) {
            snprintf(key_sysfs_node, MAX_BUF_LENTH, "event%d", index);
        }
        clog(LOG_NOTICE, "Got driver path: %s-----value is %s", driver_path, driver_name);
    }
    closedir(dir);
}

int init_getevent()
{
    int res;
    nfds = 1;
    ufds = (pollfd *)calloc(1, sizeof(ufds[0]));
    ufds[0].fd = inotify_init();
    ufds[0].events = POLLIN;

    res = inotify_add_watch(ufds[0].fd, INPUT_DEVICE_PATH, IN_DELETE | IN_CREATE);
    if(res < 0) {
        return -1;
    }

    // Init touch driver node, first pickup related sysfs nodes for touch event, then monitor it. 
    // FIXME: Once the touch driver has been changed, sysfs should be also changed.
    init_input_driver_info();

    res = scan_dir(INPUT_DEVICE_PATH);
    if(res < 0) {
        return -1;
    }
    return 0;
}

void uninit_getevent()
{
    int i;
    for(i = 0; i < nfds; i++) {
        close(ufds[i].fd);
    }
    free(ufds);
    ufds = 0;
    nfds = 0;
}

struct __input_event{
    int value;
    time_t time;
};
struct __input_event inputEvent;


static void sigcancel_handle(int signum)
{
    if (signum == SIGCANCEL)
        pthread_exit(NULL);
}

void timerHandle(sigval_t v)
{
    clog(LOG_DEBUG, "input event timed out!");
    inputEvent.value = 0;
    wakeup_ppd();

    return;
}


int startTimer(long milliseconds, timer_t tid)
{
    struct itimerspec ts, ots;
    long seconds, nseconds;

    seconds = milliseconds/1000;
    nseconds = (milliseconds)*1000*1000;

    ts.it_value.tv_sec = seconds;
    ts.it_value.tv_nsec = nseconds;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    if (timer_settime (tid, 0, &ts, &ots) < 0)
    {
        clog(LOG_ERR, "timer_settime");
        return -1;
    }
    return 0;

}


timer_t createTimer(int id)
{
    timer_t tid;
    struct sigevent se;

    memset(&se, 0, sizeof (se));
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = timerHandle;
    se.sigev_value.sival_int = id;

    if (timer_create (CLOCK_REALTIME, &se, &tid) < 0)
    {
        clog(LOG_ERR, "timer_create failed");
        return -1;
    }
    clog(LOG_INFO, "timer_create successfully.");

    return tid;
}

/*  Waits for android events on the file descriptor opened previously.
 *  This function uses poll(2) to wait for readable data.
 */
static void *event_wait (void __UNUSED__ *arg) {

    static const int EVENT_BUFFER_SIZE = 1024;

    clog(LOG_INFO, "input event thread running.");
    timer_t tid = createTimer(1);
    int current_slot = 0; // by default, slot is 0
    int slot_status = 0;

    struct input_event* event = (struct input_event*) malloc(sizeof(struct input_event));
    int res;
    int i;
    int pollres;
    const char *device_path = "/dev/input";
    while(1) {
        pollres = poll(ufds, nfds, -1);
        if (pollres == 0) {
            free(event);
            return NULL;
        }
        if(ufds[0].revents & POLLIN) {
            read_notify(device_path, ufds[0].fd);
        }
        for(i = 1; i < nfds; i++) {
            if(ufds[i].revents) {
                if(ufds[i].revents & POLLIN) {
                    res = read(ufds[i].fd, event, sizeof(struct input_event));
                    if(res < (int)sizeof(struct input_event)) {
                        clog(LOG_ERR, "Could not get event");
                        free(event);
                        return NULL;
                    }
                    clog(LOG_INFO, "Got EVENT code=0x%x, value=0x%x", event->code, event->value);
                    if (event->type == EV_ABS) {
                        // once we have received input event, boosting!!!
                        // the boosting would last until finger has left the screen,
                        // then step the timer to boosting the duration time
                        if(slot_status == 0 && inputEvent.value == 0) {
                            inputEvent.value = 1;
                            clog(LOG_NOTICE, "First input event, wakeup ppd");
                            slot_status |= 1; // mark the bit
                            wakeup_ppd();
                        }

                        if (event->code == ABS_MT_SLOT) { //Got slot info, slot value: 0, 1, .....
                            clog(LOG_INFO, "Got new slot info code=0x%x, value=0x%x", event->code, event->value);
                            slot_status |= 1 << event->value; // mark the bit
                            current_slot = event->value; // Switch to new slot, mark current slot
                        } else if (event->code == ABS_MT_TRACKING_ID && event->value == -1) { // Got cancel event
                            clog(LOG_INFO, "Got touch event cancel code=0x%x, value=0x%x", event->code, event->value);
                            slot_status = slot_status & (~(1 << current_slot));
                            if (slot_status == 0) {
                                clog(LOG_DEBUG, "No touch on screen, start/refresh timer");
                                startTimer(INPUTEVENT_EXPIRE_TIME, tid);
                            }
                        }
                        clog(LOG_INFO, "Slot status is 0x%x", slot_status);
                    } else if (event->type == EV_KEY && event->value == 1) {
                        inputEvent.value = 1;
                        startTimer(INPUTEVENT_EXPIRE_TIME, tid);
                        wakeup_ppd();
                    }
                }
            }
        }
    }
    free(event);
    return NULL;
}
/*  static int input_event_init(void)
 *
 *  create a thread to read input event
 */
static int input_event_init(void) {

    struct sigaction act;
    int ret;

    //init event monitor
    ret = init_getevent();
    if (ret < 0) {
        clog(LOG_ERR, "input event monitor init failed!!!");
        return -1;
    }

    memset(&inputEvent, 0, sizeof(inputEvent));
    act.sa_handler = sigcancel_handle;
    act.sa_flags = SA_ONESHOT;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCANCEL, &act, &prev_action);

    /* launch event thread */
    if ((ret = pthread_create(&event_thread, NULL, &event_wait, NULL))) {
        clog(LOG_ERR, "Unable to launch thread: %s\n", strerror(ret));
        sigaction(SIGCANCEL, &prev_action, NULL);
        return -1;
    }
    sigaddset(&act.sa_mask, SIGCANCEL);
    return pthread_sigmask(SIG_BLOCK, &act.sa_mask, NULL);

}

static int input_event_exit(void) {
    //release event monitor
    uninit_getevent();

    if (event_thread) {
        clog(LOG_DEBUG, "killing event thread.\n");
        int ret = pthread_kill(event_thread, SIGCANCEL);
        if (ret != 0)
            clog(LOG_ERR, "Couldn't cancel event thread (%s).\n",
                    strerror(ret));
        /* cleanup */
        ret = pthread_join(event_thread, NULL);
        if (ret != 0)
            clog(LOG_ERR, "Couldn't join event thread (%s).\n",
                    strerror(ret));
        event_thread = 0;
    }

    sigaction(SIGCANCEL, &prev_action, NULL);
    clog(LOG_INFO, "android_event exited.\n");
    return 0;

}


/*
 *  parse the 'input_event' keywork
 */
static int input_event_parse(const char *ev, void **obj) {
    int *ret = (int*)malloc(sizeof(int));
    if (ret == NULL) {
        clog(LOG_ERR, "couldn't make enough room for input_event (%s)\n",
                strerror(errno));
        return -1;
    }

    *ret = 0;

    if (strncmp(ev, "1", 1) == 0) {
        *ret = 1;
    } else if (strncmp(ev, "0", 1) == 0) {
        *ret = 0;
    } else {
        clog(LOG_ERR, "couldn't parse %s\n", ev);
        free(ret);
        return -1;
    }

    clog(LOG_DEBUG, "parsed: %s\n", *ret==1 ? "1" : "0");

    *obj = ret;
    return 0;
}

/*
 *  evaluate the 'input_event' keyword
 */
static int input_event_evaluate(const void *s) {
    const int *input = (const int *)s;

    clog(LOG_DEBUG, "called: rule_val=%d [input=%d]\n", *input, inputEvent.value);

    return (*input == inputEvent.value) ? MATCH : DONT_MATCH;
}

static PowerPolicyDaemonKeyword kw[] = {
    { "input_event", 0, 1, &input_event_parse, &input_event_evaluate, NULL, NULL, NULL, NULL, NULL},
    { NULL, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};

static PowerPolicyDaemonPlugin input_event = {
    "input",
    TAG_INPUT,
    kw,
    &input_event_init,
    &input_event_exit,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

PowerPolicyDaemonPlugin *create_plugin (void) {
    return &input_event;
}
