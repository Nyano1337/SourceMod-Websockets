typedef ConnectCallback = function void (WebSocket ws, any data);
typedef DisconnectCallback = function void (WebSocket ws, any data);
typedef ReadCallback = function void (WebSocket ws, WS_JSON json, any data);


methodmap WebSocket < Handle
{
	public static native WebSocket FromURL(const char[] url);
	public static native WebSocket Create(const char[] address, const char[] path, const int port);
	public static native WebSocket CreateSSL(const char[] address, const char[] path, const int port);
	public native void Connect();
	public native void SetHeader(const char[] header, const char[] value);
	public native void Close();
	public native void Write(WS_JSON json);
	public native void SetReadCallback(int type, ReadCallback callback, any data = 0);
	public native void SetDisconnectCallback(DisconnectCallback callback, any data = 0);
	public native void SetConnectCallback(ConnectCallback callback, any data = 0);
}