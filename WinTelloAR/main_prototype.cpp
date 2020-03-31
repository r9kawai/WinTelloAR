/**
* @brief
* WinTelloAR main prototype
* main_prototype.cpp
*/
#include "std.h"

/**
* @brief
* main function
*/
int _main(int argc, char* argv[])
 {
	std::cout << "ffmpeg_test.cpp main()" << std::endl;
	std::cout << "Boost version:" << BOOST_LIB_VERSION << std::endl;
	std::cout << "OpenCV version:" << CV_VERSION << std::endl;
	std::cout << "FFmpeg" << avutil_configuration() << avutil_license << std::endl;
	boost::system::error_code err;
	const unsigned char CMD_REQ_IFRAME[11] = { 0xcc, 0x58, 0x00, 0x7c, 0x60, 0x25, 0x00, 0x00, 0x00, 0x6c, 0x95 };
	WSAData wsadata;
	if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0) {
		std::cout << "inet err" << std::endl;
		return -1;
	}

	char video_buff[2048];
	char status_buff[2048];
	char status[256];
	char cmd[2048];

	SOCKET sock_cmd;
	sock_cmd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_cmd == INVALID_SOCKET) {
		std::cout << "cmd socket err : " << WSAGetLastError() << std::endl;
		return -1;
	}
	struct sockaddr_in sock_cmd_addr;
	sock_cmd_addr.sin_family = AF_INET;
	sock_cmd_addr.sin_port = htons(8889);
	sock_cmd_addr.sin_addr.S_un.S_addr = inet_addr("192.168.10.1");
	u_long ioctl_val_cmd = 1;
	ioctlsocket(sock_cmd, FIONBIO, &ioctl_val_cmd);

	for (int p = 0; p < 3; p++) {
		int rc = recv(sock_cmd, status_buff, sizeof(status_buff), 0);
		if (rc < 1) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				std::cout << p << " recv cmd : non" << std::endl;
			}
		}
		else {
			memset(status, 0, sizeof(status));
			strncpy(status, status_buff, rc);
			std::cout << p << " recv cmd(" << rc << ") :" << status << std::endl;
		}
		memset(cmd, 0, sizeof(cmd));
		strcpy(cmd, "command");
		sendto(sock_cmd, cmd, (int)strlen(cmd) + 1, 0, (struct sockaddr*) & sock_cmd_addr, (int)sizeof(sock_cmd_addr));
		Sleep(100);

		memset(cmd, 0, sizeof(cmd));
		strcpy(cmd, "streamon");
		sendto(sock_cmd, cmd, (int)strlen(cmd) + 1, 0, (struct sockaddr*) & sock_cmd_addr, (int)sizeof(sock_cmd_addr));
		Sleep(100);
	}

	SOCKET sock_video = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_video == INVALID_SOCKET) {
		std::cout << "video socket err : " << WSAGetLastError() << std::endl;
		return -1;
	}
	struct sockaddr_in sock_video_addr;
	sock_video_addr.sin_family = AF_INET;
	sock_video_addr.sin_port = htons(11111);
	sock_video_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(sock_video, (struct sockaddr*)& sock_video_addr, sizeof(sock_video_addr)) != 0) {
		std::cout << "video bind err : " << WSAGetLastError() << std::endl;
		return -1;
	}
	u_long ioctl_val_video = 1;
	ioctlsocket(sock_video, FIONBIO, &ioctl_val_video);

	const char* outfilename = "out.mp4";
	std::remove(outfilename);
	std::ofstream out_video_file;
	out_video_file.open(outfilename, std::ios::out|std::ios::trunc|std::ios::binary);

	for (int i = 0; i < 300; i++) {
		if ((i % 50) == 0) {
			sendto(sock_cmd, (const char*)CMD_REQ_IFRAME, sizeof(CMD_REQ_IFRAME), 0, (struct sockaddr*) & sock_cmd_addr, sizeof(sock_cmd_addr));
			Sleep(10);
		}
		if ((i % 20) == 0) {
			memset(cmd, 0, sizeof(cmd));
			strcpy(cmd, "battery?");
			sendto(sock_cmd, cmd, (int)strlen(cmd) + 1, 0, (struct sockaddr*) & sock_cmd_addr, (int)sizeof(sock_cmd_addr));
			Sleep(10);
		}

		memset(status_buff, 0, sizeof(status_buff));
		int rc = recv(sock_cmd, status_buff, sizeof(status_buff), 0);
		if (rc > 0) {
			memset(status, 0, sizeof(status));
			strncpy(status, status_buff, rc);
			std::cout << i << " recv cmd(" << rc << ") :" << status << std::endl;
		}

		while (1) {
			memset(video_buff, 0, sizeof(video_buff));
			int rv = recv(sock_video, video_buff, sizeof(video_buff), 0);
			if (rv > 0) {
				out_video_file.write(video_buff, rv);
				//std::cout << i << " recv video(" << rv << ") :" << std::endl;
			}
			else {
				break;
			}
		}

		Sleep(100);
	}

	out_video_file.close();
	closesocket(sock_video);
	closesocket(sock_cmd);
	WSACleanup();
	return 0;
}

// EOF