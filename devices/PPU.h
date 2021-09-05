#pragma once

#include "devices/Bus.h"
#include "machine/Device.h"


class PPU : public Device
{
friend class DMA;

private:
	Bus &bus;
	static const uint32_t colors[64];

	struct Control
	{
		bool nametable_x;
		bool nametable_y;
		bool vram_increment;
		bool sprite_pattern;
		bool bg_pattern;
		bool sprite_size;
		bool slave_mode;
		bool nmi_enable;
	};

	struct Mask
	{
		bool greyscale;
		bool bg_left_render;
		bool sprite_left_render;
		bool bg_render;
		bool sprite_render;
		bool red_enhance;
		bool green_enhance;
		bool blue_enhance;
	};

	struct Status
	{
		bool sprite_overflow;
		bool sprite_hit;
		bool vblank;
	};

	struct Address
	{
		bool nametable_x;
		bool nametable_y;
		uint8_t coarse_x : 5;
		uint8_t coarse_y : 5;
		uint8_t fine_y : 3;
	};

	struct OAEntry
	{
		uint8_t y;
		uint8_t id;
		uint8_t attr;
		uint8_t x;
	};

	// Registers
	Control ctrl;
	Mask    mask;
	Status  status;
	Address ppu_addr;
	Address tmp_addr;
	uint8_t oam_addr;
	uint8_t ppu_data;
	uint8_t fine_x;
	bool    w_latch;

	// OAM
	OAEntry oam[64];
	uint8_t read_oam(uint8_t addr);
	void    write_oam(uint8_t addr, uint8_t data);

	// Render
	uint16_t bg_pattern_lo;
	uint16_t bg_pattern_hi;
	uint8_t  bg_attr;
	OAEntry  sprite_oam[8];
	uint8_t  sprite_pattern_lo[8];
	uint8_t  sprite_pattern_hi[8];
	uint8_t  sprite_count;
	bool     sprite_zero;
	void     render_background();
	void     render_sprite();
	void     render_pixel();

	// Status
	int x;
	int y;

	// IO
	uint32_t output[240][256];

public:
	PPU(Bus &bus) : bus(bus) {}

	// Device interface
	uint8_t read(uint16_t addr) override;
	void write(uint16_t addr, uint8_t data) override;

	// Status
	bool is_nmi() const { return ctrl.nmi_enable && status.vblank; }

	// IO
	const uint32_t* get_output() const { return output[0]; }

	// Signals
	void reset();
	void clock();
};
