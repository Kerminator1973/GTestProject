#include "./../ccnet/transport.h"
#include <vector>

int main() {

	std::vector<uint8_t> identification{0x02, 0x03, 0x06, 0x37, 0x00, 0x00};
	std::span<uint8_t> _span(identification);
	
	auto crc16 = CCNetTransport::CalcCRC(_span);
		
	auto loBy = uint8_t(crc16 & 0x00FF);
	auto hiBy = uint8_t((crc16 & 0xFF00) >> 8);

	if (0xfe == loBy && 0xc7 == hiBy) {
		return 0;	// OK
	}
		
	return 1;	// Error
}