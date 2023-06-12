/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "it_test_process.h"
#include "sys/shm.h"

static int *g_shmptr = NULL;
static int g_usetTestCount = 0;
static VOID *PthreadTest115(VOID *arg)
{
    g_usetTestCount++;
    return NULL;
}

static int TestThread(void *arg)
{
    int data = *((int *)arg);
    int ret = 0;
    pthread_t gTh;
    ret = pthread_create(&gTh, NULL, PthreadTest115, NULL);
    ICUNIT_ASSERT_EQUAL_NULL(ret, 0, ret);

    ret = pthread_join(gTh, NULL);
    ICUNIT_ASSERT_EQUAL_NULL(ret, 0, ret);
    ICUNIT_ASSERT_EQUAL_NULL(g_usetTestCount, 1, g_usetTestCount);

    *g_shmptr = 100; // 100, set shared num.

    pid_t pid = fork();

    ICUNIT_GOTO_WITHIN_EQUAL(pid, 0, 100000, pid, EXIT); // 100000, assert pid equal to this.
    if (pid == 0) {
        sleep(1);
        exit(0);
    }

    *g_shmptr = 200; // 200, set shared num.

    ret = waitpid(pid, NULL, 0);
    printf("waitpid ret : %d errno : %d pid : %d getpid : %d\n", ret, errno, pid, getpid());

    ICUNIT_ASSERT_EQUAL_NULL(ret, pid, ret);

    *g_shmptr = data;
EXIT:
    return NULL;
}

// This testcase us used for undefination of LOSCFG_USER_TEST_SMP
static int Testcase(void)
{
    int arg = 0x2000;
    int status;
    int ret;
    char *stackTop;
    char *stack;
    pid_t pid;

    g_usetTestCount = 0;
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600); // // 0600, set shmget config.
    ICUNIT_ASSERT_NOT_EQUAL(shmid, -1, shmid);

    g_shmptr = (int *)shmat(shmid, nullptr, 0);
    ICUNIT_ASSERT_NOT_EQUAL(g_shmptr, (int *)-1, g_shmptr);

    *g_shmptr = 0;

    stack = (char *)malloc(arg);
    ICUNIT_GOTO_NOT_EQUAL(stack, NULL, stack, EXIT1);

    stackTop = (char *)((unsigned long)stack + arg);
    pid = clone(TestThread, (void *)stackTop, CLONE_VFORK, &arg);

    ICUNIT_GOTO_EQUAL(*g_shmptr, arg, *g_shmptr, EXIT2);

    ret = waitpid(pid, &status, NULL);
    ICUNIT_GOTO_EQUAL(ret, pid, ret, EXIT2);

EXIT2:
    free(stack);

EXIT1:
    shmdt(g_shmptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

void ItTestProcess049(void)
{
    TEST_ADD_CASE("IT_POSIX_PROCESS_049", Testcase, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
