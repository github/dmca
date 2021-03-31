#pragma once

struct unitytls_interface;
class UnityTLS
{
public:
	static unitytls_interface* unitytlsinterface;
	static unitytls_interface* GetUnityTLSInterface();
};