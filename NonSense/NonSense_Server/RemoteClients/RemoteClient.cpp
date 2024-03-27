#include "RemoteClient.h"

concurrent_unordered_map<RemoteClient*, shared_ptr<RemoteClient>> RemoteClient::remoteClients;
shared_ptr<RemoteClient> lastClient;