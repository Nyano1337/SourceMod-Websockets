 /**
  * SourceMod Encrypted Socket Extension
  * Copyright (C) 2020  Dreae
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details. 
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "extension.hpp"
#include <atomic>

WebSocketExtension extension;
SMEXT_LINK(&extension);

WebSocketBase *WebSocketBase::head = NULL;
std::atomic<bool> unloaded;

JSONHandler g_JSONHandler;
HandleType_t htJSON;

JSONObjectKeysHandler g_JSONObjectKeysHandler;
HandleType_t htJSONObjectKeys;

bool WebSocketExtension::SDK_OnLoad(char *error, size_t err_max, bool late)
{
	// sharesys->AddDependency(myself, "socket.ext", true, true);
    WebSocketBase *head = WebSocketBase::head;
    while (head) {
        head->OnExtLoad();
        head = head->next;
    }
	
	HandleAccess haJSON;
	haJSON.access[HandleAccess_Clone] = 0;
	haJSON.access[HandleAccess_Delete] = 0;
	haJSON.access[HandleAccess_Read] = 0;
	
	// Create a Handle Type for BigInt
	htJSON = handlesys->CreateType("WS_JSON", &g_JSONHandler, 0, NULL, &haJSON, myself->GetIdentity(), NULL);
	htJSONObjectKeys = handlesys->CreateType("WS_JSONObjectKeys", &g_JSONObjectKeysHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	
	sharesys->AddNatives(myself, json_natives);
    
    unloaded.store(false);
    return true;
}

void WebSocketExtension::SDK_OnUnload() {
    WebSocketBase *head = WebSocketBase::head;
    while (head) {
        head->OnExtUnload();
        head = head->next;
    }
    unloaded.store(true);
	handlesys->RemoveType(htJSON, myself->GetIdentity());
	handlesys->RemoveType(htJSONObjectKeys, myself->GetIdentity());
}

void log_msg(void *msg) {
    if (!unloaded.load()) {
        smutils->LogMessage(myself, reinterpret_cast<char *>(msg));
    }
    free(msg);
}


void log_err(void *msg) {
    if (!unloaded.load()) {
        smutils->LogError(myself, reinterpret_cast<char *>(msg));
    }
    free(msg);
}

void WebSocketExtension::LogMessage(const char *msg, ...) {
    char *buffer = reinterpret_cast<char *>(malloc(3072));
    va_list vp;
    va_start(vp, msg);
    vsnprintf(buffer, 3072, msg, vp);
    va_end(vp);

    smutils->AddFrameAction(&log_msg, reinterpret_cast<void *>(buffer));
}

void WebSocketExtension::LogError(const char *msg, ...) {
    char *buffer = reinterpret_cast<char *>(malloc(3072));
    va_list vp;
    va_start(vp, msg);
    vsnprintf(buffer, 3072, msg, vp);
    va_end(vp);
    
    smutils->AddFrameAction(&log_err, reinterpret_cast<void *>(buffer));
}

void execute_cb(void *cb) {
    std::unique_ptr<std::function<void()>> callback(reinterpret_cast<std::function<void()> *>(cb));
    callback->operator()();
}

void WebSocketExtension::Defer(std::function<void()> callback) {
    std::unique_ptr<std::function<void()>> cb = std::make_unique<std::function<void()>>(callback);
    smutils->AddFrameAction(&execute_cb, cb.release());
}

void JSONHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	json_decref((json_t *)object);
}

void JSONObjectKeysHandler::OnHandleDestroy(HandleType_t type, void *object)
{
	delete (struct JSONObjectKeys *)object;
}