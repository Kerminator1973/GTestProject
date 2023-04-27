#pragma once
#include "transport.h"

struct CCNetDeviceIdentification
{
	std::string PartNumber;		// Модель прибора BVS
	std::string ModuleNumber;	// УИН
	uint32_t SoftwareVersion;	// Версия хранится в числовом виде, для простоты сравнения
	uint32_t NotebaseVersion;
};

class CCNetDevice : public CCNetTransport
{
public:
	CCNetDevice(const std::string& strPortName, const int msTimeout = 2000);

	bool Identification(CCNetDeviceIdentification& identification);
};

