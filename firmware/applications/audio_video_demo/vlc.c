#include <rtthread.h>
#include <finsh.h>

int vlc(int argc, char** argv)
{
	extern int vlcview(const char *ip, int port);

	vlcview("10", 5000);

	return 0;
}
MSH_CMD_EXPORT(vlc, vlc demo);
