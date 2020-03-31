/**
* @brief
* WinTelloAR Main source
* Main.cpp
*/
#include "std.h"
#include "Main.h"
using namespace wintelloar;

/**
* @brief
* Fixed values
*/
const int Main::TELLO_PORT = 8889;
const char* Main::TELLO_IP = "192.168.10.1";
const char* Main::PING_CMD = "ping -n 1 ";
const char* Main::PHASE_NAME[] = {	"PHASE_INIT", "PHASE_PING", "PHASE_SOCK", "PHASE_CONNECT", "PHASE_ONVIDEO", "PHASE_LAND",
									"PHASE_FLIGHT", "PHASE_CLOSE", "PHASE_DONE" };
const char* Main::TELLO_CMD[] = {	"command", "streamon", "streamoff", "iframe", "battery?", "height?", "takeoff", "land",
									"speed %d", "cw %d", "ccw %d", "forward %d", "back %d", "up %d", "down %d", "left %d", "right %d" };

/**
* @brief
*/
Main::Main()
{
	SecureZeroMemory(m_phase_counter, sizeof(m_phase_counter));
	m_now_phase = Main::PHASE_INIT;
	m_phase_func[PHASE_INIT] = &Main::Pinit;
	m_phase_func[PHASE_PING] = &Main::Pping;
	m_phase_func[PHASE_SOCK] = &Main::Psock;
	m_phase_func[PHASE_CONNECT] = &Main::Pconnect;
	m_phase_func[PHASE_ONVIDEO] = &Main::Ponvideo;
	m_phase_func[PHASE_LAND] = &Main::Pland;
	m_phase_func[PHASE_FLIGHT] = &Main::Pflight;
	m_phase_func[PHASE_CLOSE] = &Main::Pclose;

	SecureZeroMemory(m_responce, sizeof(m_responce));

	m_sock_cmd_addr.sin_family = AF_INET;
	m_sock_cmd_addr.sin_port = htons(TELLO_PORT);
	m_sock_cmd_addr.sin_addr.S_un.S_addr = inet_addr(TELLO_IP);
	return;
}

/**
* @brief
*/
Main::~Main()
{
	return;
}

/**
* @brief
*/
int Main::Do()
{
	m_phase_counter[m_now_phase]++;
	(this->*m_phase_func[m_now_phase])();
	if (m_now_phase == PHASE_DONE) {
		std::cout << "Done [WinTelloAR] ---------------------" << std::endl;
		return 0;
	}
	else {
		return 1;
	}
}

/**
* @brief
*/
void Main::PrintPhase()
{
	std::cout << PHASE_NAME[m_now_phase] << " : " << m_phase_counter[m_now_phase] << std::endl;
	return;
}

/**
* @brief
*/
void Main::Pinit(void)
{
	std::cout << "Start [WinTelloAR] ---------------------" << std::endl;
	std::cout << "  Boost version : " << BOOST_LIB_VERSION << std::endl;
	std::cout << "  OpenCV version : " << CV_VERSION << std::endl;
	std::cout << "  FFmpeg version : " << avutil_configuration() << avutil_license << std::endl;
	PrintPhase();
	m_now_phase = PHASE_PING;
	return;
}

/**
* @brief
*/
void Main::Pping(void)
{
	PrintPhase();

	std::string cmd(PING_CMD);
	cmd += TELLO_IP;
	int pingcmdr = system(cmd.c_str());
	if (pingcmdr == 0) {
		m_now_phase = PHASE_SOCK;
	}
	else {
		Sleep(1000);
	}
	return;
}

/**
* @brief
*/
void Main::Psock(void)
{
	PrintPhase();

	if (WSAStartup(MAKEWORD(2, 0), &m_wsadata) != 0) {
		std::cout << "  network err" << std::endl;
		m_now_phase = PHASE_CLOSE;
		return;
	}
	m_sock_cmd = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock_cmd == INVALID_SOCKET) {
		std::cout << "  socket err : " << WSAGetLastError() << std::endl;
		m_now_phase = PHASE_CLOSE;
		return;
	}
	u_long ioctl_val_cmd = 1;
	ioctlsocket(m_sock_cmd, FIONBIO, &ioctl_val_cmd);
	for (int p = 0; p < 3; p++) {
		SockRcvStatus();

		SockSendCmd(CMD_COMMAND, 0);
		Sleep(100);
		SockRcvStatus();

		SockSendCmd(CMD_STREAMON, 0);
		Sleep(100);
		SockRcvStatus();

		SockSendCmd(CMD_GET_BATTERY, 0);
		Sleep(100);
		SockRcvStatus();

		SockSendCmd(CMD_GET_HEIGHT, 0);
		Sleep(100);
		SockRcvStatus();
	}

	if (m_responce[RES_OK]) {
		m_now_phase = PHASE_CONNECT;
	}
	else {
		m_now_phase = PHASE_PING;
		closesocket(m_sock_cmd);
		WSACleanup();
	}
	return;
}

/**
* @brief
*/
void Main::Pconnect(void)
{
	PrintPhase();

	m_now_phase = PHASE_ONVIDEO;
	return;
}

/**
* @brief
*/
void Main::Ponvideo(void)
{
	PrintPhase();

	m_now_phase = PHASE_LAND;
	return;
}

/**
* @brief
*/
void Main::Pland(void)
{
	PrintPhase();

	m_now_phase = PHASE_FLIGHT;
	return;
}

/**
* @brief
*/
void Main::Pflight()
{
	PrintPhase();

	m_now_phase = PHASE_CLOSE;
	return;
}

/**
* @brief
*/
void Main::Pclose(void)
{
	PrintPhase();
	closesocket(m_sock_cmd);
	WSACleanup();

	m_now_phase = PHASE_DONE;
	return;
}

/**
* @brief
*/
void Main::SockSendCmd(int cmd, int val)
{
	SecureZeroMemory(m_sock_cmd_buff, sizeof(m_sock_cmd_buff));
	if (cmd > CMD_LAND) {
		sprintf(m_sock_cmd_buff, TELLO_CMD[cmd], val);
	}
	else {
		strcpy(m_sock_cmd_buff, TELLO_CMD[cmd]);
	}
	int send_size = (int)strlen(m_sock_cmd_buff);
	int rc = sendto(m_sock_cmd, m_sock_cmd_buff, send_size, 0, (struct sockaddr*) & m_sock_cmd_addr, (int)sizeof(m_sock_cmd_addr));
	if (rc != send_size) {
		std::cout << "  send cmd err : " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "  send cmd : " << m_sock_cmd_buff << std::endl;
	}
	m_last_cmd = cmd;
	return;
}

/**
* @brief
*/
int Main::SockRcvStatus()
{
	SecureZeroMemory(m_sock_status_buff, sizeof(m_sock_status_buff));
	int rc = recv(m_sock_cmd, m_sock_status_buff, sizeof(m_sock_status_buff), 0);
	if (rc < 1) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			std::cout << "  recv cmd : non" << std::endl;
		}
		else {
			std::cout << "  recv cmd err : " << WSAGetLastError() << std::endl;
		}
	}
	else {
		if (strncmp(m_sock_status_buff, "ok", 2) == 0) {
			m_responce[RES_OK] = 1;
		}
		else {
			m_sock_status_buff[3] = 0;
			int val = atoi(m_sock_status_buff);
			if ((val > 0) && (m_last_cmd == CMD_GET_BATTERY)) {
				m_responce[RES_BATTERY] = val;
			}
			if ((val > 0) && (m_last_cmd == CMD_GET_HEIGHT)) {
				m_responce[RES_HEIGHT] = val;
			}
		}

		std::cout << "  recv cmd(" << rc << ") : " << m_sock_status_buff
			<< " ok." << m_responce[RES_OK]
			<< " battery." << m_responce[RES_BATTERY]
			<< " height." << m_responce[RES_HEIGHT] << std::endl;
	}
	return (int)rc;
}

// EOF