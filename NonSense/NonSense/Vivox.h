#include <iostream>
#include <Vxc.h>
#include <VxcErrors.h>
#include <VxcResponses.h>
#include <VxcRequests.h>

class VivoxSystem
{
private:
	char* key = "zing795";
	long long SerialNum = 0;
	vx_sdk_config_t defaultConfig;
	vx_message_base_t* VX_Message = NULL;

	VX_HANDLE m_ConnectorHandle = NULL;
	VX_HANDLE m_AccountHandle = NULL;
	VX_HANDLE m_SessionHandle = NULL;
public:


public:
	VivoxSystem() {}
	~VivoxSystem() {}

	void Initialize();
	void Uninitialize();
	void Listen();
	void MessageHandle(vx_message_base_t* msg);
	void ResponseHandle(vx_resp_base_t* resp);
	void EventHandle(vx_evt_base_t* evt);
	void CreateConnector(vx_resp_connector_create_t* resp);
	void Connect();
	void Disconnect();
	void JoinChannel(const char* Channel);
	void LeaveChannel(const char* Channel);


	int RequestIssue(vx_req_base_t* request);

};