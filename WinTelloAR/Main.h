/**
* @brief
* WinTelloAR Main header
* Main.h
*/
#pragma once
#include "Video.h"

int main(int argc, char* argv[]);

namespace wintelloar
{
	/**
	* @brief
	*/
	class Main
	{
	public:
		typedef void (Main::* PHASE_FUNC)(void);
		static const int TELLO_PORT;
		static const int TELLO_VIDEO_PORT;
		static const char* TELLO_IP;
		static const char* PING_CMD;
		static const char* PHASE_NAME[];
		static const char* TELLO_CMD[];
		static const char* VBUFFNAME;

		enum phase {
			PHASE_INIT,
			PHASE_PING,
			PHASE_SOCK,
			PHASE_CONNECT,
			PHASE_ONVIDEO,
			PHASE_LAND,
			PHASE_FLIGHT,
			PHASE_CLOSE,
			PHASE_DONE,
			PHASE_NUM,
		};

		enum tello_cmd {
			CMD_COMMAND,
			CMD_STREAMON,
			CMD_STREAMOFF,
			CMD_IFRAME,
			CMD_GET_BATTERY,
			CMD_GET_HEIGHT,
			CMD_TAKEOFF,
			CMD_LAND,
			CMD_SPEED_X,
			CMD_RCW_X,
			CMD_RCCW_X,
			CMD_FORWARD_X,
			CMD_BACK_X,
			CMD_UP_X,
			CMD_DOWN_X,
			CMD_LEFT_X,
			CMD_RIGHT_X,
			CMD_NUM,
		};

		enum tello_res {
			RES_OK,
			RES_BATTERY,
			RES_HEIGHT,
			RES_NUM
		};

	public:
		Main();
		~Main();
		int Do();
	
	protected:
		Video* m_vdecoder;
		std::ofstream m_vbufstream;
		int m_vbuf_totalinsize;
		char m_video_buff[2048];
		PHASE_FUNC m_phase_func[PHASE_NUM];
		int m_now_phase;
		int m_phase_counter[PHASE_NUM];
		int m_responce[RES_NUM];
		int m_last_cmd;

		WSAData m_wsadata;
		SOCKET m_sock_cmd;
		SOCKET m_sock_video;
		struct sockaddr_in m_sock_cmd_addr;
		struct sockaddr_in m_sock_video_addr;
		char m_sock_cmd_buff[256];
		char m_sock_status_buff[256];

		void PrintPhase();
		void Pinit();
		void Pping();
		void Psock();
		void Pconnect();
		void Ponvideo();
		void Pland();
		void Pflight();
		void Pclose();
		void SockSendCmd(int cmd, int val);
		int SockRcvStatus();
	};
};
// EOF