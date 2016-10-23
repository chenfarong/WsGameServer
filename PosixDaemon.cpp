/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

//#include "Config/Config.h"
#include "PosixDaemon.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <string>


void create_pid_file(const char *path);

pid_t parent_pid = 0, sid = 0;


void daemonSignal(int s)
{

    if (getpid() != parent_pid)
    {
        return;
    }

    if (s == SIGUSR1)
    {
        exit(EXIT_SUCCESS);
    }

    if (sid)
    {
        kill(sid, s);
    }

    exit(EXIT_FAILURE);
}


void startDaemon(const char* pidFname)
{
    parent_pid = getpid();
    pid_t pid;

    signal(SIGUSR1, daemonSignal);
    signal(SIGINT, daemonSignal);
    signal(SIGTERM, daemonSignal);
    signal(SIGALRM, daemonSignal);

    sid = pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

/*
    if (pid > 0)
    {
        alarm(timeout);
        pause();
        exit(EXIT_FAILURE);
    }
*/


    umask(0);

    sid = setsid();

    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0)
    {
        exit(EXIT_FAILURE);
    }

    freopen("/dev/null", "rt", stdin);
    freopen("/dev/null", "wt", stdout);
    freopen("/dev/null", "wt", stderr);


//	CreatePIDFile("/var/tmp/mcs");
	create_pid_file(pidFname);

}

void stopDaemon(const char* pidFname)
{
//    std::string pidfile;// = sConfig.GetStringDefault("PidFile", "");
    if (pidFname && strlen(pidFname)>0)
    {
        std::fstream pf(pidFname, std::ios::in);
        uint32_t pid = 0;
        pf >> pid;
        if (kill(pid, SIGKILL) < 0)
        {
            std::cerr << "Unable to stop daemon:"<< pid << std::endl;
            exit(EXIT_FAILURE);
        }
        pf.close();
    }
    else
    {
        std::cerr << "No pid file specified" << std::endl;
    }

    exit(EXIT_SUCCESS);
}

void detachDaemon()
{
    if (parent_pid)
    {
        kill(parent_pid, SIGUSR1);
    }
}


void exitDaemon()
{
    if (parent_pid && parent_pid != getpid())
    {
        kill(parent_pid, SIGTERM);
    }
}


struct WatchDog
{
    ~WatchDog()
    {
        exitDaemon();
    }
};

WatchDog dog;

//typedef unsigned int uint32;

/// create PID file
//unsigned int CreatePIDFile(const std::string& filename)
//{
//    FILE* pid_file = fopen(filename.c_str(), "w");
//    if (pid_file == nullptr)
//        return 0;
//
//#ifdef WIN32
//
//    DWORD pid = GetCurrentProcessId();
//#else
//    pid_t pid = getpid();
//    //pid_t pid = sid;
//#endif
//
//    fprintf(pid_file, "%d\n", pid);
//    fclose(pid_file);
//
//    return (unsigned int)pid;
//}

void create_pid_file(const char *path)
{
		 char buf[256];
			int fd;

		sprintf(buf,"%d\n",getpid());

			fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0644);
				if (fd == -1)
				{
							printf("open(%s) failed\n", path);
							exit(1);
				}
				write(fd, buf, strlen(buf));
		close(fd);
}


