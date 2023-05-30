#include <iostream>
#include <Vxc.h>
#include <VxcErrors.h>
#include <VxcResponses.h>
#include <VxcRequests.h>

class VivoxSystem
{
public:
	vx_sdk_config_t defaultConfig;
	vx_message_base_t* VX_Message;

public:
	void Initialize();
	void Listen();
	void MessageHandle(vx_message_base_t* msg);
	void ResponseHandle(vx_resp_base_t* resp);
	void EventHandle(vx_evt_base_t* evt);
	void CreateConnector(vx_resp_connector_create_t* resp);
	void Connect();
};