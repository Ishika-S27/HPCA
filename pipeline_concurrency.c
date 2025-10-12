Part A: Pipeline using Threads + Shared Buffers
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
int in1 = 0, out1 = 0, in2 = 0, out2 = 0;
int count1 = 0, count2 = 0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t full1 = PTHREAD_COND_INITIALIZER, empty1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t full2 = PTHREAD_COND_INITIALIZER, empty2 = PTHREAD_COND_INITIALIZER;

void *stage1(void *arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex1);
        while (count1 == BUFFER_SIZE) pthread_cond_wait(&empty1, &mutex1);

        buffer1[in1] = i;
        printf("Stage 1: Produced %d\n", i);

        in1 = (in1 + 1) % BUFFER_SIZE;
        count1++;

        pthread_cond_signal(&full1);
        pthread_mutex_unlock(&mutex1);

        sleep(1);
    }
    return NULL;
}

void *stage2(void *arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex1);
        while (count1 == 0) pthread_cond_wait(&full1, &mutex1);

        int val = buffer1[out1];
        out1 = (out1 + 1) % BUFFER_SIZE;
        count1--;

        pthread_cond_signal(&empty1);
        pthread_mutex_unlock(&mutex1);

        int result = val * val;

        pthread_mutex_lock(&mutex2);
        while (count2 == BUFFER_SIZE) pthread_cond_wait(&empty2, &mutex2);

        buffer2[in2] = result;
        printf("Stage 2: Squared %d → %d\n", val, result);

        in2 = (in2 + 1) % BUFFER_SIZE;
        count2++;

        pthread_cond_signal(&full2);
        pthread_mutex_unlock(&mutex2);

        sleep(1);
    }
    return NULL;
}

void *stage3(void *arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex2);
        while (count2 == 0) pthread_cond_wait(&full2, &mutex2);

        int val = buffer2[out2];
        out2 = (out2 + 1) % BUFFER_SIZE;
        count2--;

        pthread_cond_signal(&empty2);
        pthread_mutex_unlock(&mutex2);

        printf("Stage 3: Consumed %d\n", val);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, stage1, NULL);
    pthread_create(&t2, NULL, stage2, NULL);
    pthread_create(&t3, NULL, stage3, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
Part B: Pipeline using POSIX Message Queues 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>

#define NUM_ITEMS 10
#define QUEUE1 "/mq_stage1"
#define QUEUE2 "/mq_stage2"

void *stage1(void *arg) {
    mqd_t q1 = mq_open(QUEUE1, O_WRONLY);
    if (q1 == (mqd_t)-1) {
        perror("mq_open stage1");
        pthread_exit(NULL);
    }

    for (int i = 1; i <= NUM_ITEMS; i++) {
        if (mq_send(q1, (char*)&i, sizeof(int), 0) == -1) {
            perror("mq_send stage1");
        } else {
            printf("Stage 1: Produced %d\n", i);
        }
        sleep(1);
    }
    mq_close(q1);
    return NULL;
}

void *stage2(void *arg) {
    mqd_t q1 = mq_open(QUEUE1, O_RDONLY);
    mqd_t q2 = mq_open(QUEUE2, O_WRONLY);
    if (q1 == (mqd_t)-1 || q2 == (mqd_t)-1) {
        perror("mq_open stage2");
        pthread_exit(NULL);
    }

    for (int i = 1; i <= NUM_ITEMS; i++) {
        int val;
        if (mq_receive(q1, (char*)&val, sizeof(int), NULL) == -1) {
            perror("mq_receive stage2");
        } else {
            int result = val * val;
            mq_send(q2, (char*)&result, sizeof(int), 0);
            printf("Stage 2: Squared %d → %d\n", val, result);
        }
        sleep(1);
    }

    mq_close(q1);
    mq_close(q2);
    return NULL;
}

void *stage3(void *arg) {
    mqd_t q2 = mq_open(QUEUE2, O_RDONLY);
    if (q2 == (mqd_t)-1) {
        perror("mq_open stage3");
        pthread_exit(NULL);
    }

    for (int i = 1; i <= NUM_ITEMS; i++) {
        int val;
        if (mq_receive(q2, (char*)&val, sizeof(int), NULL) == -1) {
            perror("mq_receive stage3");
        } else {
            printf("Stage 3: Consumed %d\n", val);
        }
        sleep(1);
    }

    mq_close(q2);
    return NULL;
}

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    // Clean up old queues if they exist
    mq_unlink(QUEUE1);
    mq_unlink(QUEUE2);

    if (mq_open(QUEUE1, O_CREAT | O_RDWR, 0666, &attr) == (mqd_t)-1) {
        perror("mq_open main q1");
        exit(1);
    }
    if (mq_open(QUEUE2, O_CREAT | O_RDWR, 0666, &attr) == (mqd_t)-1) {
        perror("mq_open main q2");
        exit(1);
    }

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, stage1, NULL);
    pthread_create(&t2, NULL, stage2, NULL);
    pthread_create(&t3, NULL, stage3, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    mq_unlink(QUEUE1);
    mq_unlink(QUEUE2);
    return 0;
}


