#include <tchar.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <conio.h>

typedef enum {false, true} bool;
#define DATALEN 102

int showhelp(_TCHAR * arg0)
{
	_TCHAR *s, *p = arg0;

	while(p) {
		s = ++p;
		p = _tcschr(p, '\\');
	}

	printf(" \n");
	printf(" Version: 0.1.1 build 002\n");
	printf(" Created: 2004.02.01\n");
	printf(" Revised: 2007.07.12\n\n");
	printf(" Compiled with Borland's BCC 5.5 (Freeware) +lib: ws2_32.lib\n\n");
	printf(" SYNOPSYS:\n");
	printf(" \tWake up computers on connected LAN\n");
	printf("\n");
	printf(" USAGE:\n");
	printf(" \t%s mac-addresses...\n", s);
	printf("\n");
	printf(" ARGUMENTS:\n");
	printf(" \tmac-address: 1 or more mac address of remote computer\n");
	printf("\n");
	printf(" NOTES:\n");
	printf(" \t  MAC separator can be any of [ : - . _ / ]\n");
	printf(" \t    (semicolon, dash, dot, underscore or slash)\n");
	printf(" \t  Thus, \"ab/cd:00.1-2_3\" would be a VALID mac address.\n\n");
	printf(" \t  Any missing digit/elements will be interpreted as 0\n");
	printf("\n");
	printf(" EXAMPLES:\n\n");
	printf("   - Wake up a single computer:\n\n");
	printf("\t%s 00:aa:bb:cc:dd:ee \n\n", s);
	printf("   - Wake up computers with several separator's style:\n\n");
	printf("\t%s 00:aa:bb:cc:dd:ee 00-01-02-03-04-05 11.22/33.. \n\n", s);
	printf("   - Wake up several computers with typos:\n\n");
	printf("\t%s 701:::b3b:5c4c 192.168.1.1/16\n\n", s);
	printf("     The program will pick the last 2 characters per-element,\n");
	printf("     and fills up missing digits with zero. In this case,\n");
	printf("     it would be equal with typing command:\n\n");
	printf("\t   %s 01:00:00:3b:4c:00 92-68-01-01-16-00\n\n", s);
	//printf("\n");
	printf(" ====================================================\n");
	printf(" Copyright (c) 2003-2011\n");
	printf(" Adrian H, Ray AF & Raisa NF of PT SOFTINDO, Jakarta.\n");
	printf(" Email: aa _AT_ softindo.net\n");
	printf(" All rights reserved.\n");
	printf(" ====================================================\n");
	printf(" Press any key to continue..\n"); getch();
	return 1;
}

int showErr2(const char* msg, const bool showLastErr) {
	char * lem;
	int err = WSAGetLastError();
	printf("ERROR[%d]: %s.\n", err, msg); // no need to show last error string
	//printf("ERROR: %s.\nLast error code:%d\n%s", msg, err, lem);

	if (showLastErr) {
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lem, 0, NULL);
		if (err != 183 && err != 0) printf("%s\n", lem);
		LocalFree(lem);
	}
	return 0;
}

int shower(const char* msg) { return showErr2(msg, false); }
int showerr(const char* msg) { return showErr2(msg, true); }


int DieWithError(char *errorMessage) {
	showerr(errorMessage);
	return 0;
}

int mkmagic(const _TCHAR* arg, unsigned char * Data) {
	unsigned char nib, nab, ch, mac[8];
	int flip, i, k, len = strlen(arg);

#define MINLEN 7

	//too short
	if (len < MINLEN) return len;

	memset(mac, 0, 8 * sizeof(_TCHAR));
	k = 0; flip = 0; nab = 0;

	// printf("  making magic string: %s\n", arg);

	for (i = 0; (i < len) && k < 6; i++) {
		ch = arg[i];
		//printf("  i:%d, ch:%c, k:%d,", i, ch, k);
		switch(ch) {
			case ':': case '.': case '-': case '_': case '/':
				//printf(" written to mac[%d] = %0.2X\n", k, nab);
				mac[k++] = nab; nab = 0; flip = 0;
			continue;
			case '0':case '1':case '2':case '3':case '4':
			case '5':case '6':case '7':case '8':case '9': nib = ch - 0x30; break;
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F': nib = ch - 0x40+9; break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f': nib = ch - 0x60+9; break;
			default: return i; //break; // return errno
		}
		//printf(" nib:%0.2X, nab:%0.2X ->", nib, nab);
		flip = 1;
		nab = (nab << 4) | nib;
		//printf(" %0.2X\n", nab);
		//printf("\n");
	}
	if (flip && k < 6) {
		//printf(" peding written to mac[%d] = %0.2X\n", k, nab);
		mac[k] = nab;
	}

	//memset(Data, 0, sizeof(*Data));
	memset(Data, 0, DATALEN);
	memset(Data, 0xff, 6);

	for (i = 0; i < 16; i++)
		memmove(&Data[i*6+6], mac, 6);

	return 0;
}


int main(int argc, char *args[]) {

	INTERFACE_INFO ifInfo[32];        // max 32 interfaces

//	const DATALEN = 16*6+6;
	unsigned char DATA[DATALEN+8];

	WSADATA wsaData;
	int hsock, shock;              // Socket handle
	SOCKADDR_IN dest;                 // destination (broadcast LAN)

	unsigned long got, flags;
	int i, c, n, do_bcast, ret;
	//PSOCKADDR_IN Socket;
	SOCKET wsock;
	IN_ADDR ip, mask, bcast, bcast2;


	if (argc < 2) return showhelp(args[0]);
	//printf("Arguments count:%d\n", argc);

	/* Initialize Winsock 2.0 DLL */
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		return showerr("Can not initialize Winsock 2.2");

	wsock = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
	if (wsock == SOCKET_ERROR) return showerr("Can not create Socket");
	//printf("got socket handle: %d\n", sock);

	if (WSAIoctl(wsock, SIO_GET_INTERFACE_LIST, 0, 0, &ifInfo,
		sizeof(ifInfo), &got, 0, 0) == SOCKET_ERROR) {
			//closesocket(sock);
			return showerr("Can not call WSAIoctl");
	}

	//closesocket(shock);

	n = got / sizeof(INTERFACE_INFO);
	printf("Found %d interfaces.\n", n-1);

	//n = min(n, 1); // restrict to only 1st ethernet!

	for (c = 1; c < argc; c++) {

		//printf("Got socket handle: %d. ", hsock);
		//printf("Processing argument(%d): %s\n", c, args[c]);
		ret = mkmagic(args[c], DATA);
		if (ret) {
			printf("  Invalid argument(%d): \"%s\" pos:%d, char = '%c'\n", c, args[c], ret, args[c][ret]);
			continue;
		}
		else
		for(i = 0; i < n; i++) {
			flags = ifInfo[i].iiFlags;
			if (flags & IFF_LOOPBACK) continue;
    		ip = ((PSOCKADDR_IN) & (ifInfo[i].iiAddress))->sin_addr;
			mask = ((PSOCKADDR_IN) & (ifInfo[i].iiNetmask))->sin_addr;
			//bcast = ((PSOCKADDR_IN) & (ifInfo[i].iiBroadcastAddress))->sin_addr;
			bcast2.s_addr = ip.s_addr & mask.s_addr | ~mask.s_addr;

    		/* Create socket for sending datagrams */
			if ((hsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
				showerr("Can not create socket");
				continue;
			}

			//printf("Got socket handle: %d. ", hsock);

			do_bcast = 1;
			if (setsockopt(hsock, SOL_SOCKET, SO_BROADCAST,
				(void *) &do_bcast, sizeof(do_bcast)) < 0) {
				showerr("Fail setting socket option");
				closesocket(hsock);
				continue;
			}

			memset(&dest, 0, sizeof(dest));   // Clear dest
			dest.sin_family = AF_INET;        // Internet
			dest.sin_port = htons(9);         // port 9 = DISCARD
			dest.sin_addr = bcast2 ;          // LAN Broadcast

			if (sendto(hsock, DATA, DATALEN, 0, (PSOCKADDR) &dest, sizeof(dest)) != DATALEN)
				 showerr("Mismatched bytes sent");
			else
				printf(" - Waking up MAC %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x via %s\n",
					DATA[6],DATA[7],DATA[8],DATA[9],DATA[10],DATA[11], inet_ntoa(bcast2));
			closesocket(hsock);
		}
	}
	WSACleanup();
	return 0;
}


