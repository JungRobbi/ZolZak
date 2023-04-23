#include "RemoteClient.h"

unordered_map<RemoteClient*, shared_ptr<RemoteClient>> RemoteClient::remoteClients;
recursive_mutex RemoteClient::mx_rc;