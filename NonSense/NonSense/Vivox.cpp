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
		//std::cout << "No Message\n";
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
	else if (msg->type == msg_request)
	{
		std::cout << "------------Request-------------" << std::endl;
	}
}

void VivoxSystem::ResponseHandle(vx_resp_base_t* resp)
{
	
	switch(resp->type)
	{
	case resp_connector_create:
		//vx_resp_connector_create_t* typed_resp = reinterpret_cast<vx_resp_connector_create_t*>(resp);
		std::cout << "Connector Create MSG" << std::endl;
		if (resp->return_code == 1)
		{
			std::cout << vx_get_error_string(resp->status_code) << std::endl;
			CreateConnector(NULL);
		}
		else
		{
			Connect();
			std::cout << "Connector Create Success" << std::endl;
		}
		
		break;
	case resp_account_anonymous_login:
		std::cout << "Login Response MSG" << std::endl;
		if (resp->return_code == 1)
		{
			std::cout << vx_get_error_string(resp->status_code) << std::endl;
		}
		else
		{
			std::cout << "Login Success" << std::endl;
		}
		break;
	case resp_sessiongroup_remove_session:
		std::cout << "Remove_Session" << std::endl;
		if (resp->return_code == 1)
		{
			std::cout << vx_get_error_string(resp->status_code) << std::endl;
		}
		else
		{
			std::cout << "Remove_Session Success" << std::endl;
		}
		break;
	case resp_sessiongroup_add_session:
		std::cout << "Add Session" << std::endl;
		if (resp->return_code == 1)
		{
			std::cout << vx_get_error_string(resp->status_code) << std::endl;
		}
		else
		{
			std::cout << "Add_Session Success" << std::endl;
		}
		break;
	default:
		std::cout << "Unkown Response" << std::endl;
		break;
	}
}

void VivoxSystem::EventHandle(vx_evt_base_t* evt)
{
	std::cout << " EVENT !  " << std::endl;
}

void VivoxSystem::CreateConnector(vx_resp_connector_create_t* resp)
{
	if (!vx_is_initialized())
	{
		return;
	}
	vx_req_connector_create* req;
	vx_req_connector_create_create(&req);
	req->connector_handle = vx_strdup("https://mt1s.www.vivox.com/api2");
	req->acct_mgmt_server = vx_strdup("https://mt1s.www.vivox.com/api2");

	int vx_issue_request3_response = RequestIssue(&req->base);

}

void VivoxSystem::Connect()
{
	static long long sequence = 0;
	vx_req_account_anonymous_login_t* req;

	vx_req_account_anonymous_login_create(&req); 
	req->connector_handle = vx_strdup("https://mt1s.www.vivox.com/api2/");
	req->acct_name = vx_strdup(".jeawoo0732-no23-dev.Korus.");

	//req->acct_name = vx_get_random_user_id_ex("sa_", NULL);
	req->displayname = vx_strdup("TEST");
	req->account_handle = vx_strdup("sip:.jeawoo0732-no23-dev.Korus.@mt1s.vivox.com");
	req->access_token = vx_debug_generate_token("jeawoo0732-no23-dev",(vx_time_t)-1,"login", sequence++,NULL,"sip:.jeawoo0732-no23-dev.Korus.@mt1s.vivox.com",nullptr, (const unsigned char*)key, strlen(key));
	int vx_issue_request3_response = RequestIssue(&req->base);

}

void VivoxSystem::Disconnect()
{
	vx_req_account_logout* req;
	vx_req_account_logout_create(&req);
	req->account_handle = vx_strdup(".jeawoo0732-no23-dev.Korus.");
	int vx_issue_request3_response = RequestIssue(&req->base);
}

void VivoxSystem::JoinChannel(const char* Channel)
{
	char* uri = vx_get_echo_channel_uri(Channel, "mt1s.vivox.com", "jeawoo0732-no23-dev");
	vx_req_sessiongroup_add_session* req;
	vx_req_sessiongroup_add_session_create(&req);
	req->sessiongroup_handle = vx_strdup("sg1");
	req->session_handle = vx_strdup("Channel");
	req->uri = uri;
	req->account_handle = vx_strdup("sip:.issuer.Korus@mt1s.vivox.com");
	req->connect_audio = 1;
	req->connect_text = 1;
	req->access_token = vx_debug_generate_token("jeawoo0732-no23-dev", (vx_time_t)-1, "join", 1, NULL, "sip:.jeawoo0732-no23-dev.Korus.@mt1s.vivox.com", uri, (const unsigned char*)key, strlen(key));
	int vx_issue_request3_response = RequestIssue(&req->base);


	vx_free(uri);
}

void VivoxSystem::LeaveChannel(const char* Channel)
{
	vx_req_sessiongroup_remove_session* req;
	vx_req_sessiongroup_remove_session_create(&req);
	req->sessiongroup_handle = vx_strdup("sg1");
	req->session_handle = vx_strdup(Channel);
	int vx_issue_request3_response = RequestIssue(&req->base);
}

int VivoxSystem::RequestIssue(vx_req_base_t* req)
{
	int request_count;
	return vx_issue_request3(req, &request_count);
}
