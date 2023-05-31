#include "Vivox.h"


void VivoxSystem::Initialize()
{
	int status = vx_get_default_config3(&defaultConfig, sizeof(defaultConfig));

	if (status != VX_E_SUCCESS)
	{
		std::cout << "vx_sdk_get_default_config3() returned "<< status << ":"<< vx_get_error_string(status) << std::endl;
		return;
	}

	status = vx_initialize3(&defaultConfig, sizeof(defaultConfig));

	if(status != VX_E_SUCCESS)
	{
		std::cout << "vx_initialize3() returned " << status << ":" << vx_get_error_string(status) << std::endl;
		return;
	}
	CreateConnector(NULL);
}

void VivoxSystem::Uninitialize()
{
	vx_uninitialize();
}

void VivoxSystem::Listen()
{
	int status = vx_get_message(&VX_Message);
	if (status == VX_GET_MESSAGE_AVAILABLE)
	{
		MessageHandle(VX_Message);
		vx_destroy_message(VX_Message);
	}
	else if(status == VX_GET_MESSAGE_FAILURE)
	{
		std::cout << "Fail Message\n";
	}
	else if (status == VX_GET_MESSAGE_NO_MESSAGE)
	{
		std::cout << "No Message\n";
	}
}

void VivoxSystem::MessageHandle(vx_message_base_t* msg)
{
	if (msg->type == msg_response)
	{
		vx_resp_base_t* resp = reinterpret_cast<vx_resp_base_t*>(msg);
		ResponseHandle(resp);
	}
	else if (msg->type == msg_response)
	{
		vx_evt_base_t* evt = reinterpret_cast<vx_evt_base_t*>(msg);
		EventHandle(evt);
	}
}

void VivoxSystem::ResponseHandle(vx_resp_base_t* resp)
{
	if (resp->type == resp_connector_create)
	{
		vx_resp_connector_create_t* typed_resp = reinterpret_cast<vx_resp_connector_create_t*>(resp);
		CreateConnector(typed_resp);
	}
}

void VivoxSystem::EventHandle(vx_evt_base_t* evt)
{

}

void VivoxSystem::CreateConnector(vx_resp_connector_create_t* resp)
{
	vx_req_connector_create* req;
	vx_req_connector_create_create(&req);
	req->connector_handle = vx_strdup("c1");
	req->acct_mgmt_server = vx_strdup("https://mt1s.www.vivox.com/api2/");
	int request_count;
	int vx_issue_request3_response = vx_issue_request3(&req->base, &request_count);

}

void VivoxSystem::Connect()
{
	vx_req_account_anonymous_login_t* req;

	vx_req_account_anonymous_login_create(&req); 
	req->connector_handle = vx_strdup("c1");
	req->acct_name = vx_strdup(".jeawoo0732-no23-dev.mytestaccountname.");
	req->displayname = vx_strdup("TEST");
	req->account_handle = vx_strdup(req->acct_name);
	req->access_token = vx_debug_generate_token("jeawoo0732-no23-dev",(vx_time_t)-1,"login",1,NULL,"sip:.jeawoo0732-no23-dev.me.@mt1s.vivox.com",NULL, (const unsigned char*)key, strlen("zing795"));
	int request_count;
	vx_issue_request3(&req->base, &request_count);
}

void VivoxSystem::Disconnect()
{
	vx_req_account_logout* req;
	vx_req_account_logout_create(&req);
	req->account_handle = vx_strdup(".jeawoo0732-no23-dev.mytestaccountname.");
	int request_count;
	vx_issue_request3(&req->base, &request_count);
}

void VivoxSystem::JoinChannel(const char* Channel)
{
	char* uri = vx_get_echo_channel_uri(Channel, "mt1s.vivox.com", "jeawoo0732-no23-dev");
	vx_req_sessiongroup_add_session* req;
	vx_req_sessiongroup_add_session_create(&req);
	req->sessiongroup_handle = vx_strdup("sg1");
	req->session_handle = vx_strdup("Channel");
	req->uri = uri;
	req->account_handle = vx_strdup("sip:.issuer.playerName@mt1s.vivox.com");
	req->connect_audio = 1;
	req->connect_text = 1;
	req->access_token = vx_debug_generate_token("jeawoo0732-no23-dev", (vx_time_t)-1, "login", 1, NULL, "sip:.jeawoo0732-no23-dev.me.@mt1s.vivox.com", uri, (const unsigned char*)key, strlen("zing795"));
	int request_count;
	vx_issue_request3(&req->base, &request_count);


	vx_free(uri);
}

void VivoxSystem::LeaveChannel(const char* Channel)
{
	vx_req_sessiongroup_remove_session* req;
	vx_req_sessiongroup_remove_session_create(&req);
	req->sessiongroup_handle = vx_strdup("sg1");
	req->session_handle = vx_strdup(Channel);
	int request_count;
	vx_issue_request3(&req->base, &request_count);
}