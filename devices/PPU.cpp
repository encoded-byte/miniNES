#include "devices/PPU.h"


const uint32_t PPU::colors[64]
{
	0xff545454, 0xff001e74, 0xff081090, 0xff300088, 0xff440064, 0xff5c0030, 0xff540400, 0xff3c1800,
	0xff202a00, 0xff083a00, 0xff004000, 0xff003c00, 0xff00323c, 0xff000000, 0xff000000, 0xff000000,
	0xff989698, 0xff084cc4, 0xff3032ec, 0xff5c1ee4, 0xff8814b0, 0xffa01464, 0xff982220, 0xff783c00,
	0xff545a00, 0xff287200, 0xff087c00, 0xff007628, 0xff006678, 0xff000000, 0xff000000, 0xff000000,
	0xffeceeec, 0xff4c9aec, 0xff787cec, 0xffb062ec, 0xffe454ec, 0xffec58b4, 0xffec6a64, 0xffd48820,
	0xffa0aa00, 0xff74c400, 0xff4cd020, 0xff38cc6c, 0xff38b4cc, 0xff3c3c3c, 0xff000000, 0xff000000,
	0xffeceeec, 0xffa8ccec, 0xffbcbcec, 0xffd4b2ec, 0xffecaeec, 0xffecaed4, 0xffecb4b0, 0xffe4c490,
	0xffccd278, 0xffb4de78, 0xffa8e290, 0xff98e2b4, 0xffa0d6e4, 0xffa0a2a0, 0xff000000, 0xff000000,
};

uint8_t PPU::read_oam(uint8_t addr)
{
	uint8_t data = 0;

	switch (addr & 0x03)
	{
	case 0: data = oam[addr >> 2].y; break;
	case 1: data = oam[addr >> 2].id; break;
	case 2: data = oam[addr >> 2].attr; break;
	case 3: data = oam[addr >> 2].x; break;
	}

	return data;
}

void PPU::write_oam(uint8_t addr, uint8_t data)
{
	switch (addr & 0x03)
	{
	case 0: oam[addr >> 2].y = data; break;
	case 1: oam[addr >> 2].id = data; break;
	case 2: oam[addr >> 2].attr = data; break;
	case 3: oam[addr >> 2].x = data; break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                             DEVICE INTERFACE
//
//////////////////////////////////////////////////////////////////////////////


// Device: Read
uint8_t PPU::read(uint16_t addr)
{
	uint8_t data = 0;

	switch (addr & 0x2007)
	{
	case 0x2002:
		data |= status.sprite_overflow << 5;
		data |= status.sprite_hit << 6;
		data |= status.vblank << 7;
		status.vblank = 0;
		w_latch = 0;
		break;

	case 0x2004:
		data = read_oam(oam_addr);
		break;

	case 0x2007:
		addr = ppu_addr.coarse_x;
		addr |= ppu_addr.coarse_y << 5;
		addr |= ppu_addr.nametable_x << 10;
		addr |= ppu_addr.nametable_y << 11;
		addr |= ppu_addr.fine_y << 12;
		data = ppu_data;
		ppu_data = bus.read(addr);
		if (addr >= 0x3f00)
			data = ppu_data;
		addr += ctrl.vram_increment ? 32 : 1;
		ppu_addr.coarse_x = addr;
		ppu_addr.coarse_y = addr >> 5;
		ppu_addr.nametable_x = (addr >> 10) & 0x01;
		ppu_addr.nametable_y = (addr >> 11) & 0x01;
		ppu_addr.fine_y = addr >> 12;
		break;
	}

	return data;
}

// Device: Write
void PPU::write(uint16_t addr, uint8_t data)
{
	switch (addr & 0x2007)
	{
	case 0x2000:
		ctrl.nametable_x = data & 0x01;
		ctrl.nametable_y = data & 0x02;
		ctrl.vram_increment = data & 0x04;
		ctrl.sprite_pattern = data & 0x08;
		ctrl.bg_pattern = data & 0x10;
		ctrl.sprite_size = data & 0x20;
		ctrl.slave_mode = data & 0x40;
		ctrl.nmi_enable = data & 0x80;
		tmp_addr.nametable_x = ctrl.nametable_x;
		tmp_addr.nametable_y = ctrl.nametable_y;
		break;

	case 0x2001:
		mask.greyscale = data & 0x01;
		mask.bg_left_render = data & 0x02;
		mask.sprite_left_render = data & 0x04;
		mask.bg_render = data & 0x08;
		mask.sprite_render = data & 0x10;
		mask.red_enhance = data & 0x20;
		mask.green_enhance = data & 0x40;
		mask.blue_enhance = data & 0x80;
		break;

	case 0x2003:
		oam_addr = data;
		break;

	case 0x2004:
		write_oam(oam_addr, data);
		break;

	case 0x2005:
		if (w_latch == 0)
		{
			tmp_addr.coarse_x = data >> 3;
			fine_x = data & 0x07;
		}
		else
		{
			tmp_addr.coarse_y = data >> 3;
			tmp_addr.fine_y = data & 0x07;
		}
		w_latch = !w_latch;
		break;

	case 0x2006:
		if (w_latch == 0)
		{
			tmp_addr.coarse_y = data << 3;
			tmp_addr.nametable_x = (data >> 2) & 0x01;
			tmp_addr.nametable_y = (data >> 3) & 0x01;
			tmp_addr.fine_y = (data >> 4) & 0x03;
		}
		else
		{
			tmp_addr.coarse_x = data;
			tmp_addr.coarse_y |= data >> 5;
			ppu_addr = tmp_addr;
		}
		w_latch = !w_latch;
		break;

	case 0x2007:
		addr = ppu_addr.coarse_x;
		addr |= ppu_addr.coarse_y << 5;
		addr |= ppu_addr.nametable_x << 10;
		addr |= ppu_addr.nametable_y << 11;
		addr |= ppu_addr.fine_y << 12;
		bus.write(addr, data);
		addr += ctrl.vram_increment ? 32 : 1;
		ppu_addr.coarse_x = addr;
		ppu_addr.coarse_y = addr >> 5;
		ppu_addr.nametable_x = (addr >> 10) & 0x01;
		ppu_addr.nametable_y = (addr >> 11) & 0x01;
		ppu_addr.fine_y = addr >> 12;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                 SIGNALS
//
//////////////////////////////////////////////////////////////////////////////


// Signal: Reset
void PPU::reset()
{
	ctrl = { 0,0,0,0,0,0,0,0 };
	mask = { 0,0,0,0,0,0,0,0 };
	status = { 0,0,0 };
	ppu_addr = { 0,0,0,0,0 };
	tmp_addr = { 0,0,0,0,0 };
	oam_addr = 0;
	ppu_data = 0;
	fine_x = 0;
	w_latch = 0;
	x = -1;
	y = -1;
}

// Signal: Clock
void PPU::clock()
{
	if (y == -1 && x == 0)
	{
		status.sprite_overflow = 0;
		status.sprite_hit = 0;
		status.vblank = 0;
	}

	if (y >= -1 && y <= 239)
	{		
		render_background();
		render_sprite();
		if (y != -1 && x >= 0 && x <= 255)
			render_pixel();
	}

	if (y == 241 && x == 0)
		status.vblank = 1;

	if (++x == 340)
	{
		x = -1;
		if (++y == 261)
			y = -1;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//                                  RENDER
//
//////////////////////////////////////////////////////////////////////////////

 
// Render: Background
void PPU::render_background()
{
	bool enabled = mask.bg_render || mask.sprite_render;

	if (y == -1 && x == 304 && enabled)
	{
		ppu_addr.fine_y = tmp_addr.fine_y;
		ppu_addr.nametable_y = tmp_addr.nametable_y;
		ppu_addr.coarse_y = tmp_addr.coarse_y;
	}

	if (((x >= 8 && x <= 256) || (x >= 328 && x <= 336)) && (x & 7) == 0)
	{
		uint16_t addr = ppu_addr.nametable_y << 11;
		addr |= ppu_addr.nametable_x << 10;
		addr |= ppu_addr.coarse_y << 5;
		addr |= ppu_addr.coarse_x;
		uint8_t tile_id = bus.read(0x2000 | addr);

		addr = ppu_addr.nametable_y << 11;
		addr |= ppu_addr.nametable_x << 10;
		addr |= (ppu_addr.coarse_y >> 2) << 3;
		addr |= ppu_addr.coarse_x >> 2;
		uint8_t tile_attr = bus.read(0x23c0 | addr);

		if (ppu_addr.coarse_y & 0x02)
			tile_attr >>= 4;
		if (ppu_addr.coarse_x & 0x02)
			tile_attr >>= 2;

		addr = ctrl.bg_pattern << 12;
		addr |= tile_id << 4;
		addr |= ppu_addr.fine_y;
		uint8_t tile_pattern_lo = bus.read(addr);
		uint8_t tile_pattern_hi = bus.read(addr + 8);

		bg_pattern_lo = bg_pattern_lo << 8 | tile_pattern_lo;
		bg_pattern_hi = bg_pattern_hi << 8 | tile_pattern_hi;
		bg_attr = bg_attr << 2 | (tile_attr & 0x03);

		if (enabled && ++ppu_addr.coarse_x == 0)
			ppu_addr.nametable_x = !ppu_addr.nametable_x;
	}

	if (x == 256 && enabled)
	{
		if (++ppu_addr.fine_y == 0 && ++ppu_addr.coarse_y == 30)
		{
			ppu_addr.coarse_y = 0;
			ppu_addr.nametable_y = !ppu_addr.nametable_y;
		}
		ppu_addr.nametable_x = tmp_addr.nametable_x;
		ppu_addr.coarse_x = tmp_addr.coarse_x;
	}

	if (x == 336 || x == 338)
	{
		uint16_t addr = ppu_addr.nametable_y << 11;
		addr |= ppu_addr.nametable_x << 10;
		addr |= ppu_addr.coarse_y << 5;
		addr |= ppu_addr.coarse_x;
		bus.read(0x2000 | addr);
	}
}

// Render: Sprite
void PPU::render_sprite()
{
	bool enabled = mask.bg_render || mask.sprite_render;

	if (x == 256)
	{
		sprite_count = 0;
		sprite_zero = 0;
		for (uint8_t i = 0; i != 8; ++i)
		{
			sprite_oam[i] = { 0xff, 0xff, 0xff, 0xff };
			sprite_pattern_lo[i] = 0;
			sprite_pattern_hi[i] = 0;
		}
	}

	if (x == 256 && enabled)
	{
		for (uint8_t i = 0; i != 64; ++i)
		{
			if (y >= oam[i].y && y < oam[i].y + (ctrl.sprite_size ? 16 : 8))
			{
				if (i == 0)
					sprite_zero = 1;
				if (sprite_count < 8)
					sprite_oam[sprite_count++] = oam[i];
				else
					status.sprite_overflow = 1;
			}
		}
	}

	if (x >= 264 && x <= 320 && (x & 7) == 0 && enabled)
	{
		uint8_t i = (x - 264) >> 3;

		uint8_t table = 0;
		uint8_t tile = 0;
		if (ctrl.sprite_size == 0)
		{
			table = ctrl.sprite_pattern;
			tile = sprite_oam[i].id;
		}
		else
		{
			table = sprite_oam[i].id & 0x01;
			tile = sprite_oam[i].id & 0xfe;
			if (!(sprite_oam[i].attr & 0x80) && y >= sprite_oam[i].y + 8)
				tile++;
			if ((sprite_oam[i].attr & 0x80) && y < sprite_oam[i].y + 8)
				tile++;
		}
		uint8_t row = (y - sprite_oam[i].y) & 0x07;
		if (sprite_oam[i].attr & 0x80)
			row = 7 - row;

		uint16_t addr = (table << 12) | (tile << 4) | row;
		sprite_pattern_lo[i] = bus.read(addr);
		sprite_pattern_hi[i] = bus.read(addr + 8);
	}
}

// Render: Pixel
void PPU::render_pixel()
{
	uint8_t palette = 0;
	uint8_t pixel = 0;

	if (mask.bg_render && (mask.bg_left_render || x >= 8))
	{
		uint16_t bitmask = 0x8000 >> (fine_x + (x & 7));
		pixel  = (bg_pattern_lo & bitmask) ? 0x01 : 0;
		pixel |= (bg_pattern_hi & bitmask) ? 0x02 : 0;
		if (pixel)
			palette = (bg_attr >> (bitmask & 0xff00 ? 2 : 0)) & 0x03;
	}

	if (mask.sprite_render && (mask.sprite_left_render || x >= 8))
	{
		for (uint8_t i = 0, sprite_pixel = 0; i != sprite_count && !sprite_pixel; ++i)
		{
			if (x >= sprite_oam[i].x && x < sprite_oam[i].x + 8)
			{
				uint8_t shift = x - sprite_oam[i].x;
				if (sprite_oam[i].attr & 0x40)
					shift = 7 - shift;
				uint8_t bitmask = 0x80 >> shift;
				sprite_pixel  = (sprite_pattern_lo[i] & bitmask) ? 0x01 : 0;
				sprite_pixel |= (sprite_pattern_hi[i] & bitmask) ? 0x02 : 0;
				bool priority = sprite_oam[i].attr & 0x20;

				if (sprite_pixel && pixel && sprite_zero && i == 0 && x <= 254)
					status.sprite_hit = 1;

				if (sprite_pixel && !(pixel && priority))
				{
					pixel = sprite_pixel;
					palette = (sprite_oam[i].attr & 0x03) | 0x04;
				}
			}
		}
	}

	uint8_t color_id = bus.read(0x3f00 | palette << 2 | pixel);
	color_id &= mask.greyscale ? 0x30 : 0x3f;
	output[y][x] = colors[color_id];
}
