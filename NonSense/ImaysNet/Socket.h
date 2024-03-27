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
// SOCKET�� 64bit ȯ�濡�� 64bit�̴�. �ݸ� linux������ ������ 32bit�̴�. �� ���̸� ����.
typedef int SOCKET;
#endif

#define MAX_SOCKBUF 1024 // ��Ŷ(����� ����)ũ��
#define MAX_CLIENT 100 // �ִ� ���Ӱ����� Ŭ���̾�Ʈ ��
#define MAX_WORKERTHREAD 4 // ������ Ǯ(CP��ü)�� ���� ������ ��

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

// ���� Ŭ����
class Socket
{
public:
	SOCKET m_fd; // ���� �ڵ�

#ifdef _WIN32
	// AcceptEx �Լ� ������
	LPFN_ACCEPTEX AcceptEx = NULL;

	// Overlapped I/O�� IOCP�� �� ������ ���˴ϴ�. ���� overlapped I/O ���̸� true�Դϴ�.
	// (N-send�� N-recv�� �����ϰ� �Ϸ��� �̷��� ���� ���� �����ϸ� �ȵ�����, �� �ҽ��� ������ �н��� �켱�̹Ƿ� �̸� ������� �ʾҽ��ϴ�.)
	bool m_isReadOverlapped = false;

	// Overlapped receive or accept�� �� �� ���Ǵ� overlapped ��ü�Դϴ�. 
	// I/O �Ϸ� �������� �����Ǿ�� �մϴ�.
//	WSAOVERLAPPED m_readOverlappedStruct;
#endif
	// Receive�� ReceiveOverlapped�� ���� ���ŵǴ� �����Ͱ� ä������ ���Դϴ�.
	// overlapped receive�� �ϴ� ���� ���Ⱑ ���˴ϴ�. overlapped I/O�� ����Ǵ� ���� �� ���� �ǵ帮�� ������.
//	char m_receiveBuffer[MaxReceiveLength];

	EXP_OVER	m_recvOverlapped = EXP_OVER(); // Recv Overlapped(�񵿱�) I/O �۾��� ���� ����
	std::list<std::shared_ptr<EXP_OVER>> m_sendOverlapped_list;
	unsigned char m_prev_remain = 0;
#ifdef _WIN32
	// overlapped ������ �ϴ� ���� ���⿡ recv�� flags�� ���ϴ� ���� ä�����ϴ�. overlapped I/O�� ����Ǵ� ���� �� ���� �ǵ帮�� ������.
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
