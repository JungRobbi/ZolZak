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
	const char* Issuer = "jeawoo0732-no23-dev";
	vx_sdk_config_t defaultConfig;
	vx_message_base_t* VX_Message = NULL;

	VX_HANDLE m_ConnectorHandle = NULL;
	VX_HANDLE m_AccountHandle = NULL;
	VX_HANDLE m_SessionHandle = NULL;

	char UserName[15] = "Default";
	char* AcctName = NULL;
	char* UserURI = NULL;
public:


public:
	VivoxSystem() {
		std::string str;
		str.append(".");
		str.append(UserName);
		str.append(".");
		AcctName = new char[str.length() + 1];
		strcpy(AcctName, str.c_str());
		MakeUserURI(UserName);
	}
	VivoxSystem(const char* Name) {
		strcpy(UserName, Name);
		std::string str;
		str.append("."); 
		str.append(UserName);
		str.append(".");
		AcctName = new char[str.length()+1];
		strcpy(AcctName ,str.c_str());
		MakeUserURI(UserName);
	}
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
	void LeaveChannel();


	int RequestIssue(vx_req_base_t* request);


	void MakeUserURI(char* UserName);
};