#include <fstream>
#include <stdexcept>
#include "boards/AxROM.h"
#include "boards/Bandai74161.h"
#include "boards/CNROM.h"
#include "boards/FDS.h"
#include "boards/FCG2.h"
#include "boards/GxROM.h"
#include "boards/JF05.h"
#include "boards/JF17.h"
#include "boards/JF24.h"
#include "boards/G101.h"
#include "boards/IF07.h"
#include "boards/IF09.h"
#include "boards/IF12.h"
#include "boards/MMC1.h"
#include "boards/MMC2.h"
#include "boards/MMC3.h"
#include "boards/Namco118.h"
#include "boards/Namco163.h"
#include "boards/Namco340.h"
#include "boards/NROM.h"
#include "boards/Sunsoft1.h"
#include "boards/Sunsoft2.h"
#include "boards/Sunsoft3.h"
#include "boards/Sunsoft4.h"
#include "boards/Sunsoft5.h"
#include "boards/TC0190.h"
#include "boards/UxROM.h"
#include "boards/VRC1.h"
#include "boards/VRC3.h"
#include "boards/VRC4.h"
#include "boards/VRC6.h"
#include "boards/VRC7.h"
#include "boards/X1005.h"
#include "boards/X1017.h"
#include "devices/Cart.h"


void Cart::load(const std::string &filename)
{
	Board *type(nullptr);
	std::string extension(filename.substr(filename.find_last_of('.')));
	std::ifstream ifs(filename, std::ifstream::binary);
	if (!ifs)
		throw std::runtime_error("Cart: Failed to open " + filename);

	if (extension == ".nes")
	{
		uint8_t header[16];
		ifs.read(reinterpret_cast<char*>(header), 16);
		BoardInfo info(header);

		switch (info.mapper)
		{
		// Nintendo
		case 0:   type = new NROM(info); break;
		case 3:   type = new CNROM(info); break;
		case 185: type = new CNROMA(info); break;
		case 7:   type = new AxROM(info); break;
		case 34:  type = new BNROM(info); break;
		case 66:  type = new GxROM(info); break;
		case 2:   type = new UxROM(info); break;
		case 94:  type = new UxROMA(info); break;
		case 180: type = new UxROMB(info); break;
		case 1:   type = new MMC1(info); break;
		case 155: type = new MMC1A(info); break;
		case 9:   type = new MMC2(info); break;
		case 4:   type = new MMC3(info); break;
		case 119: type = new MMC3A(info); break;
		case 118: type = new MMC3B(info); break;
		case 10:  type = new MMC4(info); break;

		// Bandai
		case 70:  type = new Bandai74161(info); break;
		case 152: type = new Bandai74161A(info); break;
		case 16:  type = new FCG2(info); break;
		case 159: type = new FCG2(info); break;
		case 153: type = new FCG2A(info); break;

		// Irem
		case 77:  type = new IF09(info); break;
		case 97:  type = new IF07(info); break;
		case 78:  type = new IF12(info); break;
		case 32:  type = new G101(info); break;
		case 65:  type = new H3001(info); break;

		// Jaleco
		case 87:  type = new JF05(info); break;
		case 140: type = new JF11(info); break;
		case 86:  type = new JF13(info); break;
		case 72:  type = new JF17(info); break;
		case 92:  type = new JF19(info); break;
		case 18:  type = new JF24(info); break;

		// Konami
		case 75:  type = new VRC1(info); break;
		case 73:  type = new VRC3(info); break;
		case 21:  type = new VRC4(info); break;
		case 22:  type = new VRC4(info); break;
		case 23:  type = new VRC4(info); break;
		case 25:  type = new VRC4(info); break;
		case 24:  type = new VRC6(info); break;
		case 26:  type = new VRC6(info); break;
		case 85:  type = new VRC7(info); break;

		// Namco
		case 206: type = new Namco118(info); break;
		case 88:  type = new Namco118(info); break;
		case 76:  type = new Namco118A(info); break;
		case 95:  type = new Namco118B(info); break;
		case 154: type = new Namco118C(info); break;
		case 19:  type = new Namco163(info); break;
		case 210: type = new Namco340(info); break;

		// Sunsoft
		case 184: type = new Sunsoft1(info); break;
		case 93:  type = new Sunsoft2(info); break;
		case 89:  type = new Sunsoft2A(info); break;
		case 67:  type = new Sunsoft3(info); break;
		case 68:  type = new Sunsoft4(info); break;
		case 69:  type = new Sunsoft5(info); break;

		// Taito
		case 33:  type = new TC0190(info); break;
		case 48:  type = new TC0690(info); break;
		case 80:  type = new X1005(info); break;
		case 207: type = new X1005A(info); break;
		case 82:  type = new X1017(info); break;

		// Unsupported
		default: throw std::runtime_error("Cart: Unsupported mapper " + std::to_string(info.mapper));
		}
	}
	else if (extension == ".fds")
	{
		// Nintendo FDS
		type = new FDS();
	}
	else throw std::runtime_error("Cart: Unsupported file " + filename);

	board.reset(type);
	board->load(ifs);
}
