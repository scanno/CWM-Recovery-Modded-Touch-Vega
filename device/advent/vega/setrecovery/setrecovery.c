/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mtdutils/mtdutils.h>

/* Bootloader Message
 *
 * This structure describes the content of a block in flash
 * that is used for recovery and the bootloader to talk to
 * each other.
 *
 * The command field is updated by linux when it wants to
 * reboot into recovery or to update radio or bootloader firmware.
 * It is also updated by the bootloader when firmware update
 * is complete (to boot into recovery for any final cleanup)
 *
 * The status field is written by the bootloader after the
 * completion of an "update-radio" or "update-hboot" command.
 *
 * The recovery field is only written by linux and used
 * for the system to send a message to recovery or the
 * other way around.
 */
struct bootloader_message {
    char command[32];
    char status[32];
    char recovery[1024];
};

#define MISC_PAGES		3 /* number of pages to save */
#define MISC_COMMAND_PAGE	1 /* bootloader command is this page */

int
main(int argc, char *argv[])
{
    const MtdPartition *part;
    MtdReadContext *read;
    MtdWriteContext *write;
    struct bootloader_message boot;
    size_t write_size;
    ssize_t size;
    int i;
    char *data;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [message]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (mtd_scan_partitions() == -1 ||
        (part = mtd_find_partition_by_name("misc")) == NULL ||
        mtd_partition_info(part, NULL, NULL, &write_size) == -1)
        return EXIT_FAILURE;

    size = write_size * MISC_PAGES;
    if ((data = malloc(size)) == NULL)
        return EXIT_FAILURE;

    if ((read = mtd_read_partition(part)) == NULL) {
        free(data);
        return EXIT_FAILURE;
    }
    if (mtd_read_data(read, data, size) != size) {
        mtd_read_close(read);
        free(data);
        return EXIT_FAILURE;
    }
    mtd_read_close(read);

    memset(&boot, 0, sizeof(boot));
    strlcpy(boot.command, argv[1], sizeof(boot.command));
    for (i = 2; i < argc; i++) {
        strlcat(boot.recovery, argv[i], sizeof(boot.recovery));
        strlcat(boot.recovery, "\n", sizeof(boot.recovery));
    }
    memcpy(&data[write_size * MISC_COMMAND_PAGE], &boot, sizeof(boot));

    if ((write = mtd_write_partition(part)) == NULL) {
        free(data);
        return EXIT_FAILURE;
    }
    if (mtd_write_data(write, data, size) != size) {
        mtd_write_close(write);
        free(data);
        return EXIT_FAILURE;
    }
    if (mtd_write_close(write) == -1) {
        free(data);
        return EXIT_FAILURE;
    }
    free(data);

    return EXIT_SUCCESS;
}
