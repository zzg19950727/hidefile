#include "message_to_driver.h"
#include <stdio.h>

int main(void)
{
	int ch = 1;
	while (ch)
	{
		printf("0:exit\n1:start service\n2:stop service\n3:add protect file\n4:delete protect file\n5:add protect process\n6:delete protect process\nchoice:");
		scanf("%d", &ch);
		switch (ch)
		{
		case 1:
			start_service();
			break;
		case 2:
			stop_service();
			break;
		case 3:
			MyAddProtectPath(L".txt", L"ext");
			break;
		case 4:
			MyDelProtectPath(L".txt", L"ext");
			break;
		case 5:
			MyAddProtectProcess("notepad.exe");
			break;
		case 6:
			MyDelProtectProcess("notepad.exe");
			break;
		}
	}
	return 0;
}