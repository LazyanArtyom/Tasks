#include "processmanager.h"

#include "include/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>

namespace procmgr
{

void ProcessManager::exec()
{
    int fd_client, bytes_read, i;
    char buf[4096];
    char *return_fifo;
    char *numbers[100];

    proc::Fifo fifo("/tmp/fifo_server");
    int fdServer = fifo.open(O_RDONLY);
    
    while (true)
    {
        // get a message
        memset(buf, '\0', sizeof(buf));
        if ((bytes_read = read(fdServer, buf, sizeof(buf))) == -1)
            perror("read");
        if (bytes_read == 0)
            continue;

        if (bytes_read > 0)
        {
            return_fifo = strtok(buf, ", \n");

            i = 0;
            numbers[i] = strtok(NULL, ", \n");

            while (numbers[i] != NULL && i < 100)
                numbers[++i] = strtok(NULL, ", \n");

            int total_numbers = i;
            for (i = 0; i < total_numbers; i++)
            {
               std::cout << numbers[i] << std::endl; 
            }

            /* Send the result */
            if ((fd_client = open(return_fifo, O_WRONLY)) == -1)
            {
                perror("open: client fifo");
                continue;
            }

            if (error)
                sprintf(buf, "Error in input.\n");
            else
                sprintf(buf, "Good = %.8g\n", 5);

            if (write(fd_client, buf, strlen(buf)) != strlen(buf))
                perror("write");

            if (close(fd_client) == -1)
                perror("close");
        }
    }
}

} // namespace procmgr