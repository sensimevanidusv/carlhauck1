/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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


#include "It_vfs_fat.h"

static UINT32 TestCase(VOID)
{
    INT32 fd = 0;
    INT32 ret;
    CHAR pathname1[FAT_STANDARD_NAME_LENGTH] = FAT_PATH_NAME;

    ret = mkdir(pathname1, S_IRWXU | S_IRWXG | S_IRWXO);
    if (ret < 0 && errno != EEXIST) {
        ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    }

    fd = open(pathname1, O_NONBLOCK | O_CREAT | O_RDWR | O_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
    ICUNIT_GOTO_NOT_EQUAL(fd, FAT_IS_ERROR, fd, EXIT1);

    ret = fallocate(fd, FAT_FALLOCATE_KEEP_SIZE, 0, 0);
    ICUNIT_GOTO_EQUAL(ret, FAT_IS_ERROR, ret, EXIT1);

    ret = fallocate64(fd, FAT_FALLOCATE_KEEP_SIZE, 0, 0);
    ICUNIT_GOTO_EQUAL(ret, FAT_IS_ERROR, ret, EXIT1);

    ret = close(fd);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = rmdir(pathname1);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);

    return FAT_NO_ERROR;
EXIT1:
    close(fd);
EXIT:
    remove(pathname1);
    return FAT_NO_ERROR;
}

/* *
* -@test IT_FS_FAT_663
* -@tspec The function test for fallocate and fallocate64
* -@ttitle The function test for fallocate and fallocate64 that the fd open wth the directory
* -@tprecon The filesystem module is open
* -@tbrief
1. use the open to open a directory;
2. use the fallocate and fallocate64 to apply the space;
3. delete the directory;
4. N/A.
* -@texpect
1. Return successed
2. Return failed
3. Successful operation
4. N/A
* -@tprior 1
* -@tauto TRUE
* -@tremark
*/

VOID ItFsFat663(VOID)
{
    TEST_ADD_CASE("IT_FS_FAT_663", TestCase, TEST_VFS, TEST_VFAT, TEST_LEVEL2, TEST_FUNCTION);
}
