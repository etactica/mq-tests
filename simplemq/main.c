#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <mosquitto.h>
#include <sys/time.h>

#define MSGS_LOCAL 2
#define MSGS_BRIDGE 1

struct state_s {
    struct mosquitto* mosq;
    time_t start_time;
    long loop_time_millis;
    int total_msgs_sent;
    
    int msgs_local;
    int msgs_bridge;
};

int send_messages(struct state_s *st, char *topicfmt, int qos, int msg_count) {
    //printf("Sending %d messages to topic with base fmt: %s\n", msg_count, topicfmt);
    for (int i = 0; i < msg_count; i++) {
        char topic[100];
        sprintf(topic, topicfmt, i);
        mosquitto_publish(st->mosq, NULL, topic, 4, "abcd", qos, false);
    }
    st->total_msgs_sent += msg_count;
    return 0;
}

int dump_state(struct state_s *st) {
    time_t now = time(NULL);
    float msgs_per_sec = st->total_msgs_sent * 1.0 / (now - st->start_time) * 1.0;
    float loop_msgs_per_sec = (st->msgs_local + st->msgs_bridge) *1.0 / (st->loop_time_millis / 1000.0);
    printf("Looptime: %ld millis: %f msg/sec, long term: %d messages at %f msgs / sec\n", 
        st->loop_time_millis, loop_msgs_per_sec,
        st->total_msgs_sent, msgs_per_sec);
    return 0;
}

int main(int argc, char **argv) {
    struct state_s state;
    memset(&state, 0, sizeof(state));
    state.msgs_bridge = MSGS_BRIDGE;
    state.msgs_local = MSGS_LOCAL;
    if (argc > 1) {
        state.msgs_local = atoi(argv[1]);
    }
    printf("Processing %d:1 local qos2 to bridged qos0 messages\n", state.msgs_local);
    
    mosquitto_lib_init();
    state.mosq = mosquitto_new("cpu-tester-123", true, NULL);
    mosquitto_connect(state.mosq, "localhost", 1883, 60);
    mosquitto_publish(state.mosq, NULL, "status", strlen("ONLINE"), "ONLINE", 0, false);
    state.start_time = time(NULL);
    struct timeval loop_start, loop_end;
    while(1) {
        gettimeofday(&loop_start, NULL);
        mosquitto_loop(state.mosq, 100, 20);
        send_messages(&state, "local/dev-%d", 2, state.msgs_local);
        send_messages(&state, "power/bridgeddev-%d", 0, state.msgs_bridge);
        usleep(300 * 1000);
        gettimeofday(&loop_end, NULL);
        long smicros = (loop_start.tv_sec * 1000000) + (loop_start.tv_usec);
        long emicros = (loop_end.tv_sec * 1000000) + (loop_end.tv_usec);
        state.loop_time_millis = (emicros - smicros) / 1000;
        dump_state(&state);
    }

    return 1;
}
