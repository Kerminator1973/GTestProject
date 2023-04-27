#pragma once
#include "transport.h"

struct CCNetDeviceIdentification
{
	std::string PartNumber;		// ������ ������� BVS
	std::string ModuleNumber;	// ���
	uint32_t SoftwareVersion;	// ������ �������� � �������� ����, ��� �������� ���������
	uint32_t NotebaseVersion;
};

class CCNetDevice : public CCNetTransport
{
public:
	CCNetDevice(const std::string& strPortName, const int msTimeout = 2000);

	bool Identification(CCNetDeviceIdentification& identification);
};

