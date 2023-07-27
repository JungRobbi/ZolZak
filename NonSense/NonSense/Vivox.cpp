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
		//vx_destroy_message(VX_Message);
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
}

void VivoxSystem::ResponseHandle(vx_resp_base_t* resp)
{
	
	switch(resp->type)
	{
	case resp_connector_create:
		//vx_resp_connector_create_t* typed_resp = reinterpret_cast<vx_resp_connector_create_t*>(resp);
		std::cout << "Connector Create MSG : ";
		if (resp->return_code == 1)
		{
			std::cout << "Fail - " << vx_get_error_string(resp->status_code) << std::endl;
			CreateConnector(NULL);
		}
		else
		{
			std::cout << "Connector Create Success" << std::endl;
			vx_resp_connector_create_t* typed_resp = reinterpret_cast<vx_resp_connector_create_t*>(resp);
			m_ConnectorHandle = typed_resp->connector_handle;
			Connect();
		}
		
		break;
	case resp_account_anonymous_login:
		std::cout << "Login Response MSG : ";
		if (resp->return_code == 1)
		{
			std::cout << "Fail - " << vx_get_error_string(resp->status_code) << std::endl;
			Connect();
		}
		else
		{
			vx_resp_account_anonymous_login_t* typed_resp = reinterpret_cast<vx_resp_account_anonymous_login_t*>(resp);
			std::cout <<" Login Success" << std::endl;
			std::cout << "Account Handle - " << typed_resp->account_handle << std::endl;
			std::cout << "Account ID - " << typed_resp->account_id << std::endl;
			std::cout << "Account DisplayName - " << typed_resp->displayname << std::endl;
			std::cout << "URI - " << typed_resp->uri << std::endl;
			
			m_AccountHandle = typed_resp->account_handle;
		}
		break;
	case resp_sessiongroup_remove_session:
		std::cout << "Remove_Session : ";
		if (resp->return_code == 1)
		{
			std::cout << "Fail - " << vx_get_error_string(resp->status_code) << std::endl;
		}
		else
		{
			std::cout << "Remove_Session Success" << std::endl;
		}
		break;
	case resp_sessiongroup_add_session:
		std::cout << "Add Session : ";
		if (resp->return_code == 1)
		{
			std::cout << "Fail - " << vx_get_error_string(resp->status_code) << std::endl;
		}
		else
		{
			vx_resp_sessiongroup_add_session_t* typed_resp = reinterpret_cast<vx_resp_sessiongroup_add_session_t*>(resp);
			std::cout << "Add_Session Success" << std::endl;
			m_SessionHandle = typed_resp->session_handle;

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
	//req->connector_handle = vx_strdup("https://mt1s.www.vivox.com/api2");
	req->acct_mgmt_server = vx_strdup("https://mt1s.www.vivox.com/api2");

	int vx_issue_request3_response = RequestIssue(&req->base);

}

void VivoxSystem::Connect()
{
	vx_req_account_anonymous_login_t* req;

	vx_req_account_anonymous_login_create(&req); 
	req->connector_handle = m_ConnectorHandle;
	req->acct_name = vx_strdup(AcctName);
	req->displayname = vx_strdup(UserName);
	//req->account_handle = vx_strdup("sip:.jeawoo0732-no23-dev.Korus.@mt1s.vivox.com");
	req->access_token = vx_debug_generate_token("jeawoo0732-no23-dev",(vx_time_t)-1,"login", SerialNum++,NULL,
												UserURI,nullptr, (const unsigned char*)key, strlen(key));
	int vx_issue_request3_response = RequestIssue(&req->base);
}

void VivoxSystem::Disconnect()
{
	vx_req_account_logout* req;
	vx_req_account_logout_create(&req);
	
	req->account_handle = m_AccountHandle;
	int vx_issue_request3_response = RequestIssue(&req->base);
}

void VivoxSystem::JoinChannel(const char* Channel)
{
	char* uri = vx_get_general_channel_uri(Channel, "mt1s.vivox.com", "jeawoo0732-no23-dev");
	std::cout << uri << std::endl;
	//uri = vx_get_random_channel_uri_ex("confctl-e-", "mt1s.vivox.com", "jeawoo0732-no23-dev");
	vx_req_sessiongroup_add_session* req;
	vx_req_sessiongroup_add_session_create(&req);
	req->sessiongroup_handle = vx_strdup("sg1");
	req->session_handle = vx_strdup("Channel");
	req->uri = vx_strdup(uri);
	req->account_handle = m_AccountHandle;
	req->connect_audio = 1;
	req->connect_text = 1;
	req->access_token = vx_debug_generate_token("jeawoo0732-no23-dev", (vx_time_t)-1, "join", SerialNum++, NULL, UserURI, 
												uri, (const unsigned char*)key, strlen(key));
	int vx_issue_request3_response = RequestIssue(&req->base);


	vx_free(uri);
}

void VivoxSystem::LeaveChannel()
{
	vx_req_sessiongroup_remove_session* req;
	vx_req_sessiongroup_remove_session_create(&req);
	req->sessiongroup_handle = vx_strdup("sg1");
	req->session_handle = m_SessionHandle;
	int vx_issue_request3_response = RequestIssue(&req->base);
}

int VivoxSystem::RequestIssue(vx_req_base_t* req)
{
	int request_count;
	return vx_issue_request3(req, &request_count);
}

void VivoxSystem::MakeUserURI(char* UserName)
{
	std::string URI;

	URI.append("sip:");
	URI.append(AcctName);
	URI.append("@mt1s.vivox.com");
	UserURI = new char[URI.length()+1];
	strcpy(UserURI, URI.c_str());
}
