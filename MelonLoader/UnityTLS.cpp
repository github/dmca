#include "UnityTLS.h"

unitytls_interface* UnityTLS::unitytlsinterface = nullptr;
unitytls_interface* UnityTLS::GetUnityTLSInterface() { return unitytlsinterface; }