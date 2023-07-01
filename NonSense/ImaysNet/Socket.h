#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <memory>
#include <list>
#else 
#include <sys/socket.h>
#endif


#include <string>


#ifndef _WIN32
// SOCKET은 64bit 환경에서 64bit이다. 반면 linux에서는 여전히 32bit이다. 이 차이를 위함.
typedef int SOCKET;
#endif

#define MAX_SOCKBUF 1024 // 패킷(현재는 버퍼)크기
#define MAX_CLIENT 100 // 최대 접속가능한 클라이언트 수
#define MAX_WORKERTHREAD 4 // 쓰레드 풀(CP객체)에 넣을 쓰레드 수

class Endpoint;

enum class SocketType
{
	Tcp,
	Udp,
};

enum class IO_TYPE
{
	IO_RECV,
	IO_SEND,
	IO_TIMER_PLAYER_ANIMATION,
	IO_TIMER_PLAYER_MOVE,
	IO_TIMER_PLAYER_LOOK,
	IO_TIMER_MONSTER_ANIMATION,
	IO_TIMER_MONSTER_MOVE,
	IO_TIMER_MONSTER_WANDER,
	IO_TIMER_MONSTER_TARGET,

};

class EXP_OVER {
public:
	WSAOVERLAPPED _wsa_over;
	WSABUF _wsa_buf;
	char _buf[MAX_SOCKBUF];
	IO_TYPE m_ioType;
	char m_isReadOverlapped = false;

public:
	EXP_OVER() : m_ioType(IO_TYPE::IO_RECV) {
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = _buf;
		_wsa_buf.len = MAX_SOCKBUF;
		ZeroMemory(&_buf, sizeof(_buf));
	}
	EXP_OVER(const char* packet) : m_ioType(IO_TYPE::IO_SEND)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = _buf;
		_wsa_buf.len = packet[0];
		ZeroMemory(&_buf, sizeof(_buf));
		memcpy(_buf, packet, packet[0]);
	}
	EXP_OVER(const char* buf, short buf_size) : m_ioType(IO_TYPE::IO_SEND)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = _buf;
		_wsa_buf.len = buf_size;
		ZeroMemory(&_buf, sizeof(_buf));
		memcpy(_buf, buf, buf_size);
	}

	~EXP_OVER() {}
};

// 소켓 클래스
class Socket
{
public:
	SOCKET m_fd; // 소켓 핸들

#ifdef _WIN32
	// AcceptEx 함수 포인터
	LPFN_ACCEPTEX AcceptEx = NULL;

	// Overlapped I/O나 IOCP를 쓸 때에만 사용됩니다. 현재 overlapped I/O 중이면 true입니다.
	// (N-send나 N-recv도 가능하게 하려면 이렇게 단일 값만 저장하면 안되지만, 본 소스는 독자의 학습이 우선이므로 이를 고려하지 않았습니다.)
	bool m_isReadOverlapped = false;

	// Overlapped receive or accept을 할 때 사용되는 overlapped 객체입니다. 
	// I/O 완료 전까지는 보존되어야 합니다.
//	WSAOVERLAPPED m_readOverlappedStruct;
#endif
	// Receive나 ReceiveOverlapped에 의해 수신되는 데이터가 채워지는 곳입니다.
	// overlapped receive를 하는 동안 여기가 사용됩니다. overlapped I/O가 진행되는 동안 이 값을 건드리지 마세요.
//	char m_receiveBuffer[MaxReceiveLength];

	EXP_OVER	m_recvOverlapped = EXP_OVER(); // Recv Overlapped(비동기) I/O 작업을 위한 변수
	std::list<std::shared_ptr<EXP_OVER>> m_sendOverlapped_list;
	unsigned char m_prev_remain = 0;
#ifdef _WIN32
	// overlapped 수신을 하는 동안 여기에 recv의 flags에 준하는 값이 채워집니다. overlapped I/O가 진행되는 동안 이 값을 건드리지 마세요.
	DWORD m_readFlags = 0;
#endif

	Socket();
	Socket(SOCKET fd);
	Socket(SocketType socketType);
	~Socket();

	void Bind(const Endpoint& endpoint);
	void Connect(const Endpoint& endpoint);
	int Send(const char* data, int length);
	void Close();
	void Listen();
	int Accept(Socket& acceptedSocket, std::string& errorText);
#ifdef _WIN32
	bool AcceptOverlapped(Socket& acceptCandidateSocket, std::string& errorText);
	int UpdateAcceptContext(Socket& listenSocket);
#endif
	Endpoint GetPeerAddr();
	int Receive();
#ifdef _WIN32
	int ReceiveOverlapped();
	int SendOverlapped(const char* packet);
#endif
	void SetNonblocking();
	
};

std::string GetLastErrorAsString();

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#endif
