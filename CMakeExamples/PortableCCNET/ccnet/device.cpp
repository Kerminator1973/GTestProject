#include <boost/algorithm/string.hpp>
#include "device.h"

using namespace boost::algorithm;	// trim_right()


CCNetDevice::CCNetDevice(const std::string& strPortName, const int msTimeout) 
	: CCNetTransport(strPortName, msTimeout)
{
}

bool CCNetDevice::Identification(CCNetDeviceIdentification& ident)
{
	// Команда идентификации устройства имеет номер 0x37 (37H)
	auto command = std::vector<uint8_t>{ 0x02, 0x03, 0x06, 0x37, 0x00, 0x00 };
	auto success = Execute(command);
	if (success) {

		// Получаем результат
		auto data = GetResult();

		// Заполняем структуру с идентификационными параметрами
		if (data.size() < 42) {
			return false;
		}

		auto it = data.begin();

		// Пропускаем заголовок: SYNC (0x02), ADR (0x03), LNG (0x2f)
		it += 3;

		// Part Number (12 bytes): Модель прибора BVS
		ident.PartNumber = { it, it + 12 };
		trim_right(ident.PartNumber);
		it += 15;

		// Module Number (12 bytes): Уникальный Идентификационный Номер (УИН)
		ident.ModuleNumber = { it, it + 12 };
		trim_right(ident.ModuleNumber);
		it += 12;

		// Asset Number (7 bytes). На апрель 2023 года - поле не использовалось
		it += 7;

		// Software Version (4 bytes)
		{
			auto _base = 37;
			ident.SoftwareVersion = data[_base] << 24 | data[_base + 1] << 16 | data[_base + 2] << 8 | data[_base + 3];
		}

		// Note Base Version (4 bytes)
		{
			auto _base = 41;
			ident.NotebaseVersion = data[_base] << 24 | data[_base + 1] << 16 | data[_base + 2] << 8 | data[_base + 3];
		}
	}

	return success;
}