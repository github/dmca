#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../rwbase.h"
#include "../rwerror.h"
#include "../rwplg.h"
#include "../rwpipeline.h"
#include "../rwobjects.h"
#include "../rwengine.h"
#include "rwps2.h"

#define PLUGIN_ID ID_DRIVER

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

namespace rw {
namespace ps2 {

int32 nativeRasterOffset;

#define MAXLEVEL(r) ((r)->tex1low >> 2)
static bool32 noNewStyleRasters;

enum Psm {
	PSMCT32  = 0x0,
	PSMCT24  = 0x1,
	PSMCT16  = 0x2,
	PSMCT16S = 0xA,
	PSMT8    = 0x13,
	PSMT4    = 0x14,
	PSMT8H   = 0x1B,
	PSMT4HL  = 0x24,
	PSMT4HH  = 0x2C,
	PSMZ32   = 0x30,
	PSMZ24   = 0x31,
	PSMZ16   = 0x32,
	PSMZ16S  = 0x3A
};

// i don't really understand this, stolen from RW
static void
transferMinSize(int32 psm, int32 flags, int32 *minw, int32 *minh)
{
	*minh = 1;
	switch(psm){
	case PSMCT32:
	case PSMZ32:
		*minw = 2; // 32 bit
		break;
	case PSMCT16:
	case PSMCT16S:
	case PSMZ16:
	case PSMZ16S:
		*minw = 4; // 16 bit
		break;
	case PSMCT24:
	case PSMT8:
	case PSMT4:
	case PSMT8H:
	case PSMT4HL:
	case PSMT4HH:
	case PSMZ24:
		*minw = 8; // everything else
		break;
	}
	if(flags & 0x2 && psm == PSMT8){
		*minw = 16;
		*minh = 4;
	}
	if(flags & 0x4 && psm == PSMT4){
		*minw = 32;
		*minh = 4;
	}
}

#define ALIGN(x,a) ((x) + (a)-1 & ~((a)-1))
#define ALIGN16(x) ((x) + 0xF & ~0xF)
#define ALIGN64(x) ((x) + 0x3F & ~0x3F)
#define NSIZE(dim,pagedim) (((dim) + (pagedim)-1)/(pagedim))

void*
mallocalign(size_t size, int32 alignment)
{
	void *p;
	void **pp;
	p = rwMalloc(size + alignment + sizeof(void*), MEMDUR_EVENT | ID_RASTERPS2);
	if(p == nil) return nil;
	pp = (void**)(((uintptr)p + sizeof(void*) + alignment)&~(alignment-1));
	pp[-1] = p;
	return (void*)pp;
}

void
freealign(void *p)
{
	void *pp;
	if(p == nil) return;
	pp = ((void**)p)[-1];
	rwFree(pp);
}

// TODO: these depend on video mode, set in deviceSystem!
int32 cameraFormat = Raster::C8888;
int32 cameraDepth = 32;
int32 cameraZDepth = 16;

int32 defaultMipMapKL = 0xFC0;
int32 maxMipLevels = 7;

int32
getRasterFormat(Raster *raster)
{
	int32 palformat, pixelformat, mipmapflags;
	pixelformat = raster->format & 0xF00;
	palformat = raster->format & 0x6000;
	mipmapflags = raster->format & 0x9000;
	switch(raster->type){
	case Raster::ZBUFFER:
		if(palformat || mipmapflags){
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		if(raster->depth && raster->depth != cameraZDepth){
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		raster->depth = cameraZDepth;
		if(pixelformat){
			if((raster->depth == 16 && pixelformat != Raster::D16) ||
			   (raster->depth == 32 && pixelformat != Raster::D32)){
				RWERROR((ERR_INVRASTER));
				return 0;
			}
		}
		pixelformat = raster->depth == 16 ? Raster::D16 : Raster::D32;
		raster->format = pixelformat;
		break;
	case Raster::CAMERA:
		if(palformat || mipmapflags){
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		if(raster->depth && raster->depth != cameraDepth){
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		raster->depth = cameraDepth;
		if(pixelformat && pixelformat != cameraFormat){
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		pixelformat = cameraFormat;
		raster->format = pixelformat;
		break;
	case Raster::NORMAL:
	case Raster::CAMERATEXTURE:
		if(palformat || mipmapflags){
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		/* fallthrough */
	case Raster::TEXTURE:
		// Find raster format by depth if none was given
		if(pixelformat == 0)
			switch(raster->depth){
			case 4:
				pixelformat = Raster::C1555;
				palformat = Raster::PAL4;
				break;
			case 8:
				pixelformat = Raster::C1555;
				palformat = Raster::PAL8;
				break;
			case 24:
			// unsafe
			//	pixelformat = Raster::C888;
			//	palformat = 0;
			//	break;
			case 32:
				pixelformat = Raster::C8888;
				palformat = 0;
				break;
			default:
				pixelformat = Raster::C1555;
				palformat = 0;
				break;
			}
		raster->format = pixelformat | palformat | mipmapflags;
		// Sanity check raster format and depth; set depth if none given
		if(palformat){
			if(palformat == Raster::PAL8){
				if(raster->depth && raster->depth != 8){
					RWERROR((ERR_INVRASTER));
					return 0;
				}
				raster->depth = 8;
				if(pixelformat != Raster::C1555 && pixelformat != Raster::C8888){
					RWERROR((ERR_INVRASTER));
					return 0;
				}
			}else if(palformat == Raster::PAL4){
				if(raster->depth && raster->depth != 4){
					RWERROR((ERR_INVRASTER));
					return 0;
				}
				raster->depth = 4;
				if(pixelformat != Raster::C1555 && pixelformat != Raster::C8888){
					RWERROR((ERR_INVRASTER));
					return 0;
				}
			}else{
				RWERROR((ERR_INVRASTER));
				return 0;
			}
		}else if(pixelformat == Raster::C1555){
			if(raster->depth && raster->depth != 16){
				RWERROR((ERR_INVRASTER));
				return 0;
			}
			raster->depth = 16;
		}else if(pixelformat == Raster::C8888){
			if(raster->depth && raster->depth != 32){
				RWERROR((ERR_INVRASTER));
				return 0;
			}
			raster->depth = 32;
		}else if(pixelformat == Raster::C888){
			assert(0 && "24 bit rasters not supported");
			if(raster->depth && raster->depth != 24){
				RWERROR((ERR_INVRASTER));
				return 0;
			}
			raster->depth = 24;
		}else{
			RWERROR((ERR_INVRASTER));
			return 0;
		}
		break;
	default:
		RWERROR((ERR_INVRASTER));
		return 0;
	}
	return 1;
}

/*
 * Memory units:
 * Column: 64 bytes (single cycle access)
 * Block: 256 bytes, 64 words, 4 columns. texture base pointers
 * Page: 8 kbytes, 2 kwords, 128 columns, 32 blocks. frame buffer base pointers
 * entire memory: 4 mbytes, 64k columns, 16k blocks, 512 pages
 *
 * PSMT4:     128x128 pixels, 4x8 blocks per page, 32x16 pixels per block
 * PSMT8:     128x64  pixels, 8x4 blocks per page, 16x16 pixels per block
 * PSMCT16(S): 64x64  pixels, 4x8 blocks per page, 16x8  pixels per block
 * PSMCT24:    64x32  pixels, 8x4 blocks per page,  8x8  pixels per block
 * PSMCT32:    64x32  pixels, 8x4 blocks per page,  8x8  pixels per block
 *
 *   Layout of blocks in page:
 *
 *   PSMCT24, PSMCT32, PSMT8
 *   +----+----+----+----+----+----+----+----+
 *   |  0 |  1 |  4 |  5 | 16 | 17 | 20 | 21 |
 *   +----+----+----+----+----+----+----+----+
 *   |  2 |  3 |  6 |  7 | 18 | 19 | 22 | 23 |
 *   +----+----+----+----+----+----+----+----+
 *   |  8 |  9 | 12 | 13 | 24 | 25 | 28 | 29 |
 *   +----+----+----+----+----+----+----+----+
 *   | 10 | 11 | 14 | 15 | 26 | 27 | 30 | 31 |
 *   +----+----+----+----+----+----+----+----+
 *
 *   PSMCT16, PSMT4
 *   +----+----+----+----+
 *   |  0 |  2 |  8 | 10 |
 *   +----+----+----+----+
 *   |  1 |  3 |  9 | 11 |
 *   +----+----+----+----+
 *   |  4 |  6 | 12 | 14 |
 *   +----+----+----+----+
 *   |  5 |  7 | 13 | 15 |
 *   +----+----+----+----+
 *   | 16 | 18 | 24 | 26 |
 *   +----+----+----+----+
 *   | 17 | 19 | 25 | 27 |
 *   +----+----+----+----+
 *   | 20 | 22 | 28 | 30 |
 *   +----+----+----+----+
 *   | 21 | 23 | 29 | 31 |
 *   +----+----+----+----+
 *
 *   PSMCT16S
 *   +----+----+----+----+
 *   |  0 |  2 | 16 | 18 |
 *   +----+----+----+----+
 *   |  1 |  3 | 17 | 19 |
 *   +----+----+----+----+
 *   |  8 | 10 | 24 | 26 |
 *   +----+----+----+----+
 *   |  9 | 11 | 25 | 27 |
 *   +----+----+----+----+
 *   |  4 |  6 | 20 | 22 |
 *   +----+----+----+----+
 *   |  5 |  7 | 21 | 23 |
 *   +----+----+----+----+
 *   | 12 | 14 | 28 | 30 |
 *   +----+----+----+----+
 *   | 13 | 15 | 29 | 31 |
 *   +----+----+----+----+
 *
 */

static uint8 blockmap_PSMCT32[32] = {
	 0,  1,  4,  5, 16, 17, 20, 21,
	 2,  3,  6,  7, 18, 19, 22, 23,
	 8,  9, 12, 13, 24, 25, 28, 29,
	10, 11, 14, 15, 26, 27, 30, 31,
};
static uint8 blockmap_PSMCT16[32] = {
	 0,  2,  8, 10,
	 1,  3,  9, 11,
	 4,  6, 12, 14,
	 5,  7, 13, 15,
	16, 18, 24, 26,
	17, 19, 25, 27,
	20, 22, 28, 30,
	21, 23, 29, 31,
};
static uint8 blockmap_PSMCT16S[32] = {
	 0,  2, 16, 18,
	 1,  3, 17, 19,
	 8, 10, 24, 26,
	 9, 11, 25, 27,
	 4,  6, 20, 22,
	 5,  7, 21, 23,
	12, 14, 28, 30,
	13, 15, 29, 31,
};
static uint8 blockmap_PSMZ32[32] = {
	24, 25, 28, 29,  8,  9, 12, 13,
	26, 27, 30, 31, 10, 11, 14, 15,
	16, 17, 20, 21,  0,  1,  4,  5,
	18, 19, 22, 23,  2,  3,  6,  7,
};
static uint8 blockmap_PSMZ16[32] = {
	24, 26, 16, 18,
	25, 27, 17, 19,
	28, 30, 20, 22,
	29, 31, 21, 23,
	 8, 10,  0,  2,
	 9, 11,  1,  3,
	12, 14,  4,  6,
	13, 15,  5,  7,
};
static uint8 blockmap_PSMZ16S[32] = {
	24, 26,  8, 10,
	25, 27,  9, 11,
	16, 18,  0,  2,
	17, 19,  1,  3,
	28, 30, 12, 14,
	29, 31, 13, 15,
	20, 22,  4,  6,
	21, 23,  5,  7,
};

static uint8 blockmaprev_PSMCT32[32] = {
	 0,  1,  8,  9,  2,  3, 10, 11,
	16, 17, 24, 25, 18, 19, 26, 27,
	 4,  5, 12, 13,  6,  7, 14, 15,
	20, 21, 28, 29, 22, 23, 30, 31,
};
static uint8 blockmaprev_PSMCT16[32] = {
	 0,  4,  1,  5,
	 8, 12,  9, 13,
	 2,  6,  3,  7,
	10, 14, 11, 15,
	16, 20, 17, 21,
	24, 28, 25, 29,
	18, 22, 19, 23,
	26, 30, 27, 31,
};

/* Suffixes used:
 * _Px - pixels
 * _W - width units (pixels/64)
 * _B - blocks
 * _P - pages
 */

/* Layout mipmaps and palette in GS memory */
static void
calcOffsets(int32 width_Px, int32 height_Px, int32 psm, uint64 *bufferBase_B, uint64 *bufferWidth_W, uint32 *trxpos, uint32 *totalSize, uint32 *paletteBase)
{
	uint32 pageWidth_Px, pageHeight_Px;
	uint32 blockWidth_Px, blockHeight_Px;
	uint32 mindim_Px;
	int32 nlevels;
	int32 n;
	uint32 mipw_Px, miph_Px;
	uint32 lastmipw_Px, lastmiph_Px;
	uint32 bufferHeight_P[8];
	uint32 bufferPage_B[8];	// address of page in which the level is allocated
	uint32 xoff_Px, yoff_Px;	// x/y offset the last level starts at
	// Whenever we allocate horizontally inside a page,
	//  keep track of the region below it on this stack.
	uint32 sp;
	uint32 xoffstack_Px[8];	// actually unused...
	uint32 widthstack_Px[8];
	uint32 heightstack_Px[8];
	uint32 basestack_B[8];
	uint32 flag;

	switch(psm){
	case PSMCT32:
	case PSMCT24:
	case PSMT8H:
	case PSMT4HL:
	case PSMT4HH:
	case PSMZ32:
	case PSMZ24:
		pageWidth_Px = 64;
		pageHeight_Px = 32;
		blockWidth_Px = 8;
		blockHeight_Px = 8;
		break;
	case PSMT8:
		pageWidth_Px = 128;
		pageHeight_Px = 64;
		blockWidth_Px = 16;
		blockHeight_Px = 16;
		break;
	case PSMT4:
		pageWidth_Px = 128;
		pageHeight_Px = 128;
		blockWidth_Px = 32;
		blockHeight_Px = 16;
		break;
	case PSMCT16:
	case PSMCT16S:
	case PSMZ16:
	case PSMZ16S:
	default:
		pageWidth_Px = 64;
		pageHeight_Px = 64;
		blockWidth_Px = 16;
		blockHeight_Px = 8;
		break;
	}

	mindim_Px = min(width_Px, height_Px);
	for(nlevels = 1; mindim_Px > 8; nlevels++){
		if(nlevels >= maxMipLevels)
			break;
		mindim_Px /= 2;
	}

#define PAGEWIDTH_B (pageWidth_Px/blockWidth_Px)	// number of horizontal blocks per page
#define NBLKX(dim) (NSIZE((dim), blockWidth_Px))
#define NBLKY(dim) (NSIZE((dim), blockHeight_Px))
#define NPGX(dim) (NSIZE((dim), pageWidth_Px))
#define NPGY(dim) (NSIZE((dim), pageHeight_Px))
#define REALWIDTH(w) (max((w), blockWidth_Px))
#define REALHEIGHT(w) (max((w), blockHeight_Px))

	bufferBase_B[0] = 0;
	bufferWidth_W[0] = NPGX(width_Px)*pageWidth_Px/64;
	bufferHeight_P[0] = NPGY(height_Px);
	bufferPage_B[0] = 0;
	lastmipw_Px = width_Px;
	lastmiph_Px = height_Px;
	sp = 0;
	xoff_Px = 0;
	yoff_Px = 0;
	flag = 0;
	// Calculate info for all mipmap levels.
	// mipwidth/height are actually the dimensions of level n-1!
	// This code was reversed from RW and is rather complicated...
	// partially because it's not clear what the assumptions are,
	// can width/height be non-powers of 2?
	for(n = 1; n < nlevels; n++){
		mipw_Px = lastmipw_Px/2;
		miph_Px = lastmiph_Px/2;
		if(lastmipw_Px >= pageWidth_Px){
			if(lastmiph_Px >= pageHeight_Px){
				// CASE 0
				// We allocate full pages
				// This is the only place bufferWidth can change. Similarly bufferBase_2, which is related
				bufferBase_B[n] = bufferBase_B[n-1] + (lastmipw_Px/blockWidth_Px)*(lastmiph_Px/blockHeight_Px);
				bufferPage_B[n] = bufferBase_B[n];
				bufferWidth_W[n] = NPGX(mipw_Px)*pageWidth_Px/64;
				bufferHeight_P[n] = NPGY(miph_Px);
				xoff_Px = 0;
				yoff_Px = 0;
			}else{
				// CASE 1
				// Allocate vertically in the current page
				bufferPage_B[n] = bufferPage_B[n-1];
				bufferHeight_P[n] = bufferHeight_P[n-1];
				bufferWidth_W[n] = bufferWidth_W[n-1];
				// How do we know pageHeight - yoff - REALHEIGHT(lastmiph) >= miph?
				// And how is this condition ever false?
				// Assuming lastmipw >= pageWidth for any number of levels, lastmiph must be pageHeight/2
				// or lower to reach this code. No dimension is lower than 8. Then consequent mipmaps
				// will have heights halved but even with PSMT4 we will only (vertically) fill the
				// page with the last mipmap and not go beyond...
				if(REALHEIGHT(lastmiph_Px) + yoff_Px < pageHeight_Px){
					// CASE 2
					yoff_Px += REALHEIGHT(lastmiph_Px);
					bufferBase_B[n] = bufferBase_B[n-1] +
						PAGEWIDTH_B * NBLKY(lastmiph_Px) *
						bufferWidth_W[n]*64/pageWidth_Px;	// number of horizontal pages for level
				}else{
					// CASE 3
					// Can this happen?
					xoff_Px += REALWIDTH(lastmipw_Px);
					bufferBase_B[n] = bufferBase_B[n-1] + NBLKX(lastmipw_Px);
				}
			}
		}else if(lastmiph_Px >= pageHeight_Px){
			// CASE 4
			// Allocate horizontally
			bufferPage_B[n] = bufferPage_B[n-1];
			bufferHeight_P[n] = bufferHeight_P[n-1];
			bufferWidth_W[n] = bufferWidth_W[n-1];
			if(REALWIDTH(lastmipw_Px) + xoff_Px < pageWidth_Px){
				// CASE 5
				xoffstack_Px[sp] = xoff_Px;	// unused...
				heightstack_Px[sp] = REALHEIGHT(lastmiph_Px);
				widthstack_Px[sp] = REALWIDTH(lastmipw_Px);
				basestack_B[sp] = bufferBase_B[n-1] +  NBLKY(lastmiph_Px) * PAGEWIDTH_B;
				sp++;
				xoff_Px += REALWIDTH(lastmipw_Px);
				bufferBase_B[n] = bufferBase_B[n-1] + NBLKX(lastmipw_Px);
			}else if(sp){
				// CASE 7
				bufferBase_B[n] = basestack_B[sp-1];
				if(REALWIDTH(mipw_Px) < widthstack_Px[sp-1]){
					// CASE 9
					basestack_B[sp-1] += NBLKX(mipw_Px);
					widthstack_Px[sp-1] -= REALWIDTH(mipw_Px);
				}else if(REALHEIGHT(miph_Px) < heightstack_Px[sp-1]){
					// CASE 8
					basestack_B[sp-1] += NBLKY(miph_Px) * PAGEWIDTH_B;
					heightstack_Px[sp-1] -= REALHEIGHT(miph_Px);
				}else{
					// CASE 10
					sp--;
				}
				flag = 1;
			}else{
				// CASE 6
				yoff_Px += REALHEIGHT(lastmiph_Px);
				bufferBase_B[n] = bufferBase_B[n-1] + PAGEWIDTH_B*NBLKY(lastmiph_Px);
			}
		}else{
			// CASE 11
			bufferHeight_P[n] = bufferHeight_P[n-1];
			bufferPage_B[n] = bufferPage_B[n-1];
			bufferWidth_W[n] = bufferWidth_W[n-1];
			if(REALWIDTH(lastmipw_Px) + xoff_Px < bufferWidth_W[n-1]*64){
				// CASE 12
				xoffstack_Px[sp] = xoff_Px;	// unused...
				widthstack_Px[sp] = REALWIDTH(lastmipw_Px);
				heightstack_Px[sp] = REALHEIGHT(lastmiph_Px);
				basestack_B[sp] = bufferBase_B[n-1] + PAGEWIDTH_B * NBLKY(lastmiph_Px);
				sp++;
				xoff_Px += REALWIDTH(lastmipw_Px);
				bufferBase_B[n] = bufferBase_B[n-1] + NBLKX(lastmipw_Px);
			}else if(REALHEIGHT(lastmiph_Px) + yoff_Px < pageHeight_Px*bufferHeight_P[n] && flag == 0){
				// CASE 13
				bufferBase_B[n] = bufferBase_B[n-1] + PAGEWIDTH_B * NBLKY(lastmiph_Px);
				yoff_Px += blockHeight_Px ? lastmiph_Px : 0;	// how exactly can blockHeight be 0?? This looks wrong...
				flag = n;
			}else{
				// CASE 14
				bufferBase_B[n] = basestack_B[sp-1];
				if(REALWIDTH(mipw_Px) < widthstack_Px[sp-1]){
					// CASE 15
					basestack_B[sp-1] += NBLKX(mipw_Px);
					widthstack_Px[sp-1] -= REALWIDTH(mipw_Px);
				}else if(REALHEIGHT(miph_Px) < heightstack_Px[sp-1]){
					// CASE 16
					basestack_B[sp-1] += PAGEWIDTH_B * NBLKY(miph_Px);
					heightstack_Px[sp-1] -= REALHEIGHT(miph_Px);
				}else{
					// CASE 17
					sp--;
				}
			}
		}
		lastmipw_Px = mipw_Px;
		lastmiph_Px = miph_Px;
	}

	// Calculate position of palette.
	uint32 paletteBase_B = 0;
	uint64 bufwidth_Px = bufferWidth_W[nlevels-1]*64;
	uint64 bufheight_Px = bufferHeight_P[nlevels-1]*pageHeight_Px;
	// != means > really
	if(bufwidth_Px != lastmipw_Px || bufheight_Px != lastmiph_Px){
		if(psm == PSMT8){
			// 2x2 blocks at the end of the page (even for PSMCT16S)
			paletteBase_B = bufferPage_B[nlevels-1] +
				((bufwidth_Px/pageWidth_Px)*bufferHeight_P[nlevels-1] << 5)	// total number of blocks
				- (bufheight_Px/pageWidth_Px) * PAGEWIDTH_B	// one block up
				- 2;	// two blocks left
		}else if(psm == PSMT4){
			// One block at the end of the page
			paletteBase_B = bufferPage_B[nlevels-1] +
				((bufwidth_Px/pageWidth_Px) * bufferHeight_P[nlevels-1] << 5)
				- 1;
		}
	}else{
		if(psm == PSMT8 || psm == PSMT4){
			paletteBase_B = bufferPage_B[nlevels-1] +
				(bufwidth_Px/blockWidth_Px) * (bufheight_Px/blockHeight_Px);
		}
	}

	uint32 bufwidth_W = bufferWidth_W[0];
	uint32 bufpage_B = bufferPage_B[0];
	uint32 pixeloff;
	for(n = 0; n < nlevels; n++){
		// Calculate TRXPOS register (DSAX and DSAY, shifted up later)
		// Start of buffer on current page (x in pixels, y in blocks)
		pixeloff = (bufferBase_B[n] - bufpage_B) * blockWidth_Px;
		// y coordinate of first pixel
		yoff_Px = (pixeloff / (bufwidth_W*64)) * blockHeight_Px;
		// x coordinate of first pixel
		xoff_Px =  pixeloff % (bufwidth_W*64);
		if(bufferWidth_W[n] == bufwidth_W &&
		   // Not quite sure what's the meaning of this.
		   // DSAY is 11 bits, but so is DSAX and it is not checked?
		   yoff_Px < 0x800){
			trxpos[n] = yoff_Px<<16 | xoff_Px;
		}else{
			bufwidth_W = bufferWidth_W[n];
			bufpage_B = bufferPage_B[n];
			trxpos[n] = 0;
		}

		// If using more than one page we have to swizzle rows inside page rows
		if(bufwidth_W*64 / pageWidth_Px > 1){
			uint32 bufpagestride_B = bufwidth_W*64 * 32 / pageWidth_Px;	// one row of pages
			uint32 bufwidth_B = bufwidth_W*64 / blockWidth_Px;	// one row of blocks
			// To illustrate assume:
			// - 8x4 block pages
			// - texture is 4 pages wide
			// Then the lower bits of an input block address look like: RRRPPCC
			// where the C bits are the block's column inside a page
			//       the P bits are the block's page horizontally
			//       the R bits are the block's row in a row of pages
			// We want to swap P and R: PPRRRCC
			bufferBase_B[n] =
				(bufferBase_B[n] & ~((uint64)bufpagestride_B - PAGEWIDTH_B))	// mask out R and P
				| ((bufferBase_B[n] & (bufwidth_B - PAGEWIDTH_B)) * (bufpagestride_B/bufwidth_B))	// extract P and shift left
				| ((bufferBase_B[n] & (bufpagestride_B - bufwidth_B)) / (bufwidth_B/PAGEWIDTH_B));	// extract R and shift right
		}

		// Always have to swizzle blocks inside pages. We use a lookup, RW does bit operations
		switch(psm){
		case PSMCT32:
		case PSMCT24:
		case PSMT8:
		case PSMT8H:
		case PSMT4HL:
		case PSMT4HH:
			// ABCDE -> CADBE
			bufferBase_B[n] = (bufferBase_B[n]&~0x1F) | (uint64)blockmap_PSMCT32[bufferBase_B[n]&0x1F];
			break;
		case PSMT4:
		case PSMCT16:
			// ABCDE -> ADBEC
			bufferBase_B[n] = (bufferBase_B[n]&~0x1F) | (uint64)blockmap_PSMCT16[bufferBase_B[n]&0x1F];
			break;
		case PSMCT16S:
			// ABCDE -> DBAEC
			bufferBase_B[n] = (bufferBase_B[n]&~0x1F) | (uint64)blockmap_PSMCT16S[bufferBase_B[n]&0x1F];
			break;
		case PSMZ32:
		case PSMZ24:
			// ABCDE -> ~C~ADBE
			bufferBase_B[n] = (bufferBase_B[n]&~0x1F) | (uint64)blockmap_PSMZ32[bufferBase_B[n]&0x1F];
			break;
		case PSMZ16:
			// ABCDE -> ~A~DBEC
			bufferBase_B[n] = (bufferBase_B[n]&~0x1F) | (uint64)blockmap_PSMZ16[bufferBase_B[n]&0x1F];
			break;
		case PSMZ16S:
			// ABCDE -> ~D~BAEC
			bufferBase_B[n] = (bufferBase_B[n]&~0x1F) | (uint64)blockmap_PSMZ16S[bufferBase_B[n]&0x1F];
			break;
		default: break;
		}
	}

	// Same dance as above, with the palette
	if(bufwidth_W*64 / pageWidth_Px > 1){
		uint32 bufpagestride_B = bufwidth_W*64 * 32 / pageWidth_Px;	// one row of pages
		uint32 bufwidth_B = bufwidth_W*64 / blockWidth_Px;	// one row of blocks
		paletteBase_B =
			(paletteBase_B & ~(bufpagestride_B - PAGEWIDTH_B))	// mask out R and P
			| ((paletteBase_B & (bufwidth_B - PAGEWIDTH_B)) * (bufpagestride_B/bufwidth_B))	// extract P and shift left
			| ((paletteBase_B & (bufpagestride_B - bufwidth_B)) / (bufwidth_B/PAGEWIDTH_B));	// extract R and shift right
	}
	switch(psm){
	case PSMCT32:
	case PSMCT24:
	case PSMT8:
	case PSMT8H:
	case PSMT4HL:
	case PSMT4HH:
		paletteBase_B = (paletteBase_B&~0x1F) | (uint64)blockmap_PSMCT32[paletteBase_B&0x1F];
		break;
	case PSMT4:
	case PSMCT16:
		paletteBase_B = (paletteBase_B&~0x1F) | (uint64)blockmap_PSMCT16[paletteBase_B&0x1F];
		break;
	case PSMCT16S:
		paletteBase_B = (paletteBase_B&~0x1F) | (uint64)blockmap_PSMCT16S[paletteBase_B&0x1F];
		break;
	case PSMZ32:
	case PSMZ24:
		paletteBase_B = (paletteBase_B&~0x1F) | (uint64)blockmap_PSMZ32[paletteBase_B&0x1F];
		break;
	case PSMZ16:
		paletteBase_B = (paletteBase_B&~0x1F) | (uint64)blockmap_PSMZ16[paletteBase_B&0x1F];
		break;
	case PSMZ16S:
		paletteBase_B = (paletteBase_B&~0x1F) | (uint64)blockmap_PSMZ16S[paletteBase_B&0x1F];
		break;
	default: break;
	}
	*paletteBase = paletteBase_B;
	*totalSize = bufferPage_B[nlevels-1] +	// start of last buffer`
		bufferWidth_W[nlevels-1]*64/blockWidth_Px *	// number of horizontal blocks in last level
		pageHeight_Px*bufferHeight_P[nlevels-1]/blockHeight_Px;	// number of vertical blocks in last level
	*totalSize *= 64;	// to words

#undef BLKSTRIDE
#undef NBLKX
#undef NBLKY
#undef NPGX
#undef NPGY
#undef REALWIDTH
#undef REALHEIGHT
}

static Raster*
rasterCreateTexture(Raster *raster)
{
	// We use a map for fast lookup, even for impossible depths
	static int32 pageWidths[32] = {
		128, 128, 128, 128,
		128, 128, 128, 128,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
	};
	static int32 pageHeights[32] = {
		128, 128, 128, 128,
		64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64,
		32, 32, 32, 32, 32, 32, 32, 32,
		32, 32, 32, 32, 32, 32, 32, 32,
	};
	enum {
		TCC_RGBA = 1 << 2,
		CLD_1 = 1 << 29,

		WD2BLK = 64,	// words per block
		WD2PG = 2048,	// words per page
	};
	int32 pixelformat, palformat;
	// TEX0 fields (not all)
	int64 tbw	= 0;	// texture buffer width, texels/64
	int64 psm	= 0;	// pixel storage mode
	int64 tw	= 0;	// texture width exponent, width = 2^tw
	int64 th	= 0;	// texture height exponent, height = 2^th
	int64 tcc	= 0;	// texture color component, 0 = rgb, 1 = rgba
	int64 cpsm	= 0;	// CLUT pixel storage mode
	int64 cld	= 0;	// CLUT buffer load control

	uint64 bufferWidth[7];	// in number of pixels / 64
	uint64 bufferBase[7];	// block address
	uint32 trxpos_hi[8];
	int32 width, height, depth;
	int32 pageWidth, pageHeight;
	int32 paletteWidth, paletteHeight, paletteDepth;
	int32 palettePagewidth, palettePageheight;


	Ps2Raster *ras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	pixelformat = raster->format & 0xF00;
	palformat = raster->format & 0x6000;
	width = raster->width;
	height = raster->height;
	depth = raster->depth;
	// RW's code does not seem to quite work with 24 bit rasters
	// so make sure we're not generating them for safety
	assert(depth != 24);

	ras->flags = 0;
	ras->data = nil;
	ras->dataSize = 0;

	// RW doesn't seem to check this, hm...
	if(raster->flags & Raster::DONTALLOCATE)
		return raster;

	//printf("%x %x %x %x\n", raster->format, raster->flags, raster->type, noNewStyleRasters);
	pageWidth = pageWidths[depth-1];
	pageHeight = pageHeights[depth-1];

	int32 s;
	tw = 0;
	for(s = 1; s < width; s *= 2)
		tw++;
	th = 0;
	for(s = 1; s < height; s *= 2)
		th++;
	ras->kl = defaultMipMapKL;
	// unk2[0] = 1
	//printf("%d %d %d %d\n", raster->width, logw, raster->height, logh);

	// round up to page width, set TBW, TW, TH
	tbw = max(width,pageWidth)/64;

	// set PSM, TCC, CLD, CPSM and figure out palette format
	if(palformat){
		if(palformat == Raster::PAL8){
			psm = PSMT8;
			paletteWidth = 16;
			paletteHeight = 16;
		}else if(palformat == Raster::PAL4){
			psm = PSMT4;
			paletteWidth = 8;
			paletteHeight = 2;
		}else{
			// can't happen, sanity check in getRasterFormat
			return nil;;
		}
		tcc = 1;	// RGBA
		cld = 1;
		if(pixelformat == Raster::C1555){
			paletteDepth = 2;
			cpsm = PSMCT16S;
			palettePagewidth = 64;
			palettePageheight = 64;
		}else if(pixelformat == Raster::C8888){
			paletteDepth = 4;
			cpsm = PSMCT32;
			palettePagewidth = 64;
			palettePageheight = 32;
		}else
			// can't happen, sanity check in getRasterFormat
			return nil;;
	}else{
		paletteWidth = 0;
		paletteHeight = 0;
		paletteDepth = 0;
		palettePagewidth = 0;
		palettePageheight = 0;
		if(pixelformat == Raster::C8888){
			psm = PSMCT32;
			tcc = 1;	// RGBA
		}else if(pixelformat == Raster::C888){
			psm = PSMCT24;
			tcc = 0;	// RGB
		}else if(pixelformat == Raster::C1555){
			psm = PSMCT16S;
			tcc = 1;	// RGBA
		}else
			// can't happen, sanity check in getRasterFormat
			return nil;;
	}

	for(int i = 0; i < 7; i++){
		bufferWidth[i] = 1;
		bufferBase[i] = 0;
	}

	int32 mipw, miph;
	int32 w, h;
	int32 n;
	int32 nPagW, nPagH;
	raster->stride = width*depth/8;

	if(raster->format & Raster::MIPMAP){
		// NOTE: much of this code seems to be totally useless.
		// calcOffsets overwrites what we calculate here. I wonder
		// why this code even is in RW. Maybe it's older code that used
		// the GS' automatic base pointer calculation?

		// see the left columns in the maps above
		static uint32 blockOffset32_24_8[8] = { 0, 2, 2, 8, 8, 10, 10, 32 };
		static uint32 blockOffset16_4[8] = { 0, 1, 4, 5, 16, 17, 20, 21 };
		static uint32 blockOffset16S[8] = { 0, 1, 8, 9, 4, 5, 12, 13 };
		uint64 lastBufferWidth;
		mipw = width;
		miph = height;
		lastBufferWidth = max(pageWidth, width)/64;
		ras->pixelSize = 0;
		int32 lastaddress = 0;	// word address
		int32 nextaddress = 0;	// word address
		int32 stride;	// in bytes
		for(n = 0; mipw != 0 && miph != 0 && n < maxMipLevels; n++){
			if(width >= 8 && height >= 8 && (mipw < 8 || miph < 8))
				break;
			ras->pixelSize += ALIGN64(mipw*miph*depth/8);
			bufferWidth[n] = max(pageWidth, mipw)/64;
			stride = bufferWidth[n]*64*depth/8;

			// If buffer width changes, align next address to page
			if(bufferWidth[n] != lastBufferWidth){
				nPagW = ((width >> (n-1)) + pageWidth-1)/pageWidth;
				nPagH = ((height >> (n-1)) + pageHeight-1)/pageHeight;
				nextaddress = (lastaddress + nPagW*nPagH*WD2PG) & ~(WD2PG-1);
			}
			lastBufferWidth = bufferWidth[n];
			nextaddress = ALIGN64(nextaddress);	// this should already be the case...
			uint32 b = nextaddress>>(11-3) & 7;	// upper three bits of block-in-page address
			switch(psm){
			case PSMCT32:
			case PSMCT24:	
			case PSMT8:
				b = blockOffset32_24_8[b];
				break;
			case PSMCT16:
			case PSMT4:
				b = blockOffset16_4[b];
				break;
			case PSMCT16S:
				b = blockOffset16S[b];
				break;
			default:
				// can't happen
				break;
			}
			// shift to page address, then to block address and add offset inside page
			bufferBase[n] = b + (nextaddress>>11 << 5);

			lastaddress = nextaddress;
			nextaddress = ALIGN64(miph*stride/4 + lastaddress);

			mipw /= 2;
			miph /= 2;
		}

		// Do the real work here
		uint32 paletteBase;
		uint32 totalSize;
		calcOffsets(width, height, psm, bufferBase, bufferWidth, trxpos_hi, &totalSize, &paletteBase);

		ras->paletteSize = paletteWidth*paletteHeight*paletteDepth;
		ras->miptbp1 = 
			  bufferWidth[1]<<14 | (bufferBase[1] & 0x3FFF)<<0
			| bufferWidth[2]<<34 | (bufferBase[2] & 0x3FFF)<<20
			| bufferWidth[3]<<54 | (bufferBase[3] & 0x3FFF)<<40;
		ras->miptbp2 = 
			  bufferWidth[4]<<14 | (bufferBase[4] & 0x3FFF)<<0
			| bufferWidth[5]<<34 | (bufferBase[5] & 0x3FFF)<<20
			| bufferWidth[6]<<54 | (bufferBase[6] & 0x3FFF)<<40;
		ras->tex1low = (n-1)<<2;
		ras->totalSize = totalSize;
		if(ras->paletteSize){
			ras->paletteBase = paletteBase;
			if(ras->paletteBase*64 == ras->totalSize)
				ras->totalSize += WD2PG;
		}else
			ras->paletteBase = 0;
	}else{
		// No mipmaps

		ras->pixelSize = ALIGN16(raster->stride*raster->height);
		ras->paletteSize = paletteWidth*paletteHeight*paletteDepth;
		ras->miptbp1 = 1ULL<<54 | 1ULL<<34 | 1ULL<<14;
		ras->miptbp2 = 1ULL<<54 | 1ULL<<34 | 1ULL<<14;
		ras->tex1low = 0;	// one mipmap level

		// find out number of pages needed
		nPagW = (width + pageWidth-1)/pageWidth;
		nPagH = (height + pageHeight-1)/pageHeight;

		// calculate buffer width in units of pixels/64
		bufferBase[0] = 0;
		trxpos_hi[0] = 0;
		bufferWidth[0] = nPagW*pageWidth / 64;

		// calculate whole buffer size in words
		ras->totalSize = nPagW*nPagH*WD2PG;

		// calculate palette offset on GS in units of words/64
		if(ras->paletteSize){
			// Maximum palette size is 256 words.
			// If there is still room, use it!
			// If dimensions don't fill a page, we have at least
			// half a page left, enough for any palette
//TODO: this was not always done it seems but even gta3's 3.1 seems to??
			if(pageWidth*nPagW > width ||
			   pageHeight*nPagH > height){
				ras->paletteBase = (ras->totalSize - 256) / WD2BLK;
			}else{
				// Otherwise allocate more space...
				ras->paletteBase = ras->totalSize / WD2BLK;
				// ...using the same calculation as above.
				// WHY? we never need more than one page!
				nPagW = (paletteWidth + palettePagewidth-1)/palettePagewidth;
				nPagH = (paletteHeight + palettePageheight-1)/palettePageheight;
				ras->totalSize += nPagW*nPagH*WD2PG;
			}
		}else
			ras->paletteBase = 0;
	}
	ras->tex0 = tbw << 14 |
		psm << 20 |
		tw << 26 |
		th << 30 |
		tcc << 34 |
		cpsm << 51 |
		0ULL << 55 |	// csm0
		0ULL << 56 |	// entry offset
		cld << 61;



	// allocate data and fill with GIF packets
	ras->pixelSize = ALIGN16(ras->pixelSize);
	int32 numLevels = MAXLEVEL(ras)+1;
	// No GIF packet because we either don't want it (pre 0x310 rasters)
	// or the data wouldn't fit into a DMA packet
	if(noNewStyleRasters ||
	   (raster->width*raster->height*raster->depth/8/0x10) >= 0x7FFF){
		ras->dataSize = ras->paletteSize+ras->pixelSize;
		uint8 *data = (uint8*)mallocalign(ras->dataSize, 0x40);
		assert(data);
		ras->data = data;
		raster->pixels = data;
		if(ras->paletteSize)
			raster->palette = data + ras->pixelSize;
		if(raster->depth == 8)
			ras->flags |= Ps2Raster::SWIZZLED8;
	}else{
		ras->flags |= Ps2Raster::NEWSTYLE;
		uint64 paltrxpos = 0;
		uint32 dsax = trxpos_hi[numLevels-1] & 0x7FF;
		uint32 dsay = trxpos_hi[numLevels-1]>>16 & 0x7FF;
		// Set swizzle flags and calculate TRXPOS for palette
		if(psm == PSMT8){
			ras->flags |= Ps2Raster::SWIZZLED8;
			if(cpsm == PSMCT32 && bufferWidth[numLevels-1] == 2){	// one page
				// unswizzle the starting block of the last buffer and palette
				uint32 bufbase_B = (bufferBase[numLevels-1]&~0x1F) | (uint64)blockmaprev_PSMCT32[bufferBase[numLevels-1]&0x1F];
				uint32 palbase_B = (ras->paletteBase&~0x1F) | (uint64)blockmaprev_PSMCT32[ras->paletteBase&0x1F];
				// find start of page of last level (16,16 are PSMT8 block dimensions)
				uint32 page_B = bufbase_B - 8*(dsay/16) - dsax/16;
				// find palette DSAX/Y (in PSMCT32!)
				dsay = (palbase_B - page_B)/8 * 8;	// block/blocksPerPageX * blockHeight
				dsax = (palbase_B - page_B)*8 % 64;	// block*blockWidth % pageWidth
				if(dsay < 0x800)
					paltrxpos = dsay<<16 | dsax;
			}
		}
		if(psm == PSMT4){
			// swizzle flag depends on version :/
			// but which version? ....
			if(rw::version > 0x31000){
				ras->flags |= Ps2Raster::SWIZZLED4;
				// Where can this come from? if anything we're using PSMCT16S
				// Looks like they wanted to swizzle palettes too...
				if(cpsm == PSMCT16){
					// unswizzle the starting block of the last buffer and palette
					uint32 bufbase_B = (bufferBase[numLevels-1]&~0x1F) | (uint64)blockmaprev_PSMCT16[bufferBase[numLevels-1]&0x1F];
					uint32 palbase_B = (ras->paletteBase&~0x1F) | (uint64)blockmaprev_PSMCT16[ras->paletteBase&0x1F];
					// find start of page of last level (32,16 are PSMT4 block dimensions)
					uint32 page_B = bufbase_B - 4*(dsay/32) - dsax/16;
					// find palette DSAX/Y (in PSMCT16!)
					dsay = (palbase_B - page_B)/4 * 8;	// block/blocksPerPageX * blockHeight
					dsax = (palbase_B - page_B)*16 % 128;	// block*blockWidth % pageWidth
					if(dsay < 0x800)
						paltrxpos = dsay<<16 | dsax;
				}
			}
		}
		ras->pixelSize = 0x50*numLevels;	// GIF packets
		int32 minW, minH;
		transferMinSize(psm, ras->flags, &minW, &minH);
		w = raster->width;
		h = raster->height;
		n = numLevels;
		while(n--){
			mipw = max(w, minW);
			miph = max(h, minH);
			ras->pixelSize += ALIGN16(mipw*miph*raster->depth/8);
			w /= 2;
			h /= 2;
		}
		if(ras->paletteSize){
			if(rw::version > 0x31000 && paletteHeight == 2)
				paletteHeight = 3;
			ras->paletteSize = 0x50 +
			    paletteDepth*paletteWidth*paletteHeight;
		}
		// One transfer per buffer width, 4 qwords:
		// DMAcnt(2) [NOP, DIRECT]
		//   GIF tag  A+D
		//     BITBLTBUF
		// DMAref(pixel data) [NOP, DIRECT]
		uint32 extrasize = 0x10;	// PixelPtr
		int32 numTransfers = 0;
		for(n = 0; n < numLevels; n++)
			if(trxpos_hi[n] == 0){
				extrasize += 0x40;
				numTransfers++;
			}
		if(ras->paletteSize){
			extrasize += 0x40;
			numTransfers++;
		}
		// What happens here?
		if(ras->paletteSize && paltrxpos == 0)
			ras->dataSize = ALIGN(ras->pixelSize,128) + ALIGN(ras->paletteSize,64) + extrasize + 0x70;
		else
			ras->dataSize = ALIGN(ras->paletteSize+ras->pixelSize,64) + extrasize + 0x70;
		uint8 *data = (uint8*)mallocalign(ras->dataSize, 0x40);
		uint32 *xferchain = (uint32*)(data + 0x10);
		assert(data);
		ras->data = data;
		Ps2Raster::PixelPtr *pp = (Ps2Raster::PixelPtr*)data;
		pp->numTransfers = numTransfers;
		pp->numTotalTransfers = numTransfers;
		pp->pixels = (uint8*)ALIGN((uintptr)data + extrasize, 128);
		raster->pixels = (uint8*)pp;
		if(ras->paletteSize)
			raster->palette = pp->pixels + ALIGN(ras->pixelSize, 128) + 0x50;
		uint32 *p = (uint32*)pp->pixels;
		w = raster->width;
		h = raster->height;
		for(n = 0; n < numLevels; n++){
			mipw = max(w, minW);
			miph = max(h, minH);

			// GIF tag
			*p++ = 3;          // NLOOP = 3
			*p++ = 0x10000000; // NREG = 1
			*p++ = 0xE;        // A+D
			*p++ = 0;

			// TRXPOS
			if((ras->flags & Ps2Raster::SWIZZLED8 && psm == PSMT8) ||
			   (ras->flags & Ps2Raster::SWIZZLED4 && psm == PSMT4)){
				*p++ = 0;	// SSAX/Y is always 0
				*p++ = (trxpos_hi[n] & ~0x10001)/2;	// divide both DSAX/Y by 2
			}else{
				*p++ = 0;
				*p++ = trxpos_hi[n];
			}
			*p++ = 0x51;
			*p++ = 0;

			// TRXREG
			if((ras->flags & Ps2Raster::SWIZZLED8 && psm == PSMT8) ||
			   (ras->flags & Ps2Raster::SWIZZLED4 && psm == PSMT4)){
				*p++ = mipw/2;
				*p++ = miph/2;
			}else{
				*p++ = mipw;
				*p++ = miph;
			}
			*p++ = 0x52;
			*p++ = 0;

			// TRXDIR
			*p++ = 0;	// host -> local
			*p++ = 0;
			*p++ = 0x53;
			*p++ = 0;

			// GIF tag
			uint32 sz = ALIGN16(mipw*miph*raster->depth/8)/16;
			*p++ = sz & 0x7FFF;
			*p++ = 0x08000000; // IMAGE
			*p++ = 0;
			*p++ = 0;

			if(trxpos_hi[n] == 0){
				// Add a transfer, see above for layout

				*xferchain++ = 0x10000002;	// DMAcnt, 2 qwords
				*xferchain++ = 0;
				*xferchain++ = 0;		// VIF nop
				*xferchain++ = 0x50000002;	// VIF DIRECT 2 qwords

				// GIF tag
				*xferchain++ = 1;          // NLOOP = 1
				*xferchain++ = 0x10000000; // NREG = 1
				*xferchain++ = 0xE;        // A+D
				*xferchain++ = 0;

				// BITBLTBUF
				if(ras->flags & Ps2Raster::SWIZZLED8 && psm == PSMT8){
					// PSMT8 is swizzled to PSMCT32 and dimensions are halved
					*xferchain++ = PSMCT32<<24 | bufferWidth[n]/2<<16;	// src buffer
					*xferchain++ = PSMCT32<<24 | bufferWidth[n]/2<<16 | bufferBase[n];	// dst buffer
				}else if(ras->flags & Ps2Raster::SWIZZLED4 && psm == PSMT4){
					// PSMT4 is swizzled to PSMCT16 and dimensions are halved
					*xferchain++ = PSMCT16<<24 | bufferWidth[n]/2<<16;	// src buffer
					*xferchain++ = PSMCT16<<24 | bufferWidth[n]/2<<16 | bufferBase[n];	// dst buffer
				}else{
					*xferchain++ = psm<<24 | bufferWidth[n]<<16;	// src buffer
					*xferchain++ = psm<<24 | bufferWidth[n]<<16 | bufferBase[n];	// dst buffer
				}
				*xferchain++ = 0x50;
				*xferchain++ = 0;

				*xferchain++ = 0x30000000 | sz+5;	// DMAref
				// this obviously only works with 32 bit pointers, but it's only needed on the PS2 anyway
				*xferchain++ = (uint32)(uintptr)p - 0x50;
				*xferchain++ = 0;		// VIF nop
				*xferchain++ = 0x50000000 | sz+5;	// VIF DIRECT 2 qwords
			}else{
				// Add to existing transfer
				xferchain[-4] = 0x30000000 | (xferchain[-4]&0xFFFF) + sz+5;	// last DMAref
				xferchain[-1] = 0x50000000 | (xferchain[-1]&0xFFFF) + sz+5;	// last DIRECT
			}

			p += sz*4;
			w /= 2;
			h /= 2;
		}

		if(ras->paletteSize){
			// huh?
			if(paltrxpos)
				raster->palette = (uint8*)p + 0x50;
			p = (uint32*)(raster->palette - 0x50);
			// GIF tag
			*p++ = 3;          // NLOOP = 3
			*p++ = 0x10000000; // NREG = 1
			*p++ = 0xE;        // A+D
			*p++ = 0;

			// TRXPOS
			*(uint64*)p = paltrxpos;
			p += 2;
			*p++ = 0x51;
			*p++ = 0;

			// TRXREG
			*p++ = paletteWidth;
			*p++ = paletteHeight;
			*p++ = 0x52;
			*p++ = 0;

			// TRXDIR
			*p++ = 0;	// host -> local
			*p++ = 0;
			*p++ = 0x53;
			*p++ = 0;

			// GIF tag
			uint32 sz = ALIGN16(ras->paletteSize - 0x50)/16;
			*p++ = sz & 0x7FFF;
			*p++ = 0x08000000; // IMAGE
			*p++ = 0;
			*p++ = 0;

			// Transfer
			*xferchain++ = 0x10000002;	// DMAcnt, 2 qwords
			*xferchain++ = 0;
			*xferchain++ = 0;		// VIF nop
			*xferchain++ = 0x50000002;	// VIF DIRECT 2 qwords

			// GIF tag
			*xferchain++ = 1;          // NLOOP = 1
			*xferchain++ = 0x10000000; // NREG = 1
			*xferchain++ = 0xE;        // A+D
			*xferchain++ = 0;

			// BITBLTBUF
			if(paltrxpos == 0){
				*xferchain++ = cpsm<<24 | 1<<16;	// src buffer
				*xferchain++ = cpsm<<24 | 1<<16 | ras->paletteBase;	// dst buffer
				*xferchain++ = 0x50;
				*xferchain++ = 0;
			}else{
				// copy last pixel bitbltbuf...if uploading palette separately it's still the same buffer
				xferchain[0] = xferchain[-16];
				xferchain[1] = xferchain[-15];
				xferchain[2] = xferchain[-14];
				xferchain[3] = xferchain[-13];
				// Add to last transfer
				xferchain[-16] = 0x30000000 | (xferchain[-16]&0xFFFF) + sz+5;	// last DMAref
				xferchain[-13] = 0x50000000 | (xferchain[-13]&0xFFFF) + sz+5;	// last DIRECT
				xferchain += 4;
				pp->numTransfers--;
			}

			*xferchain++ = 0x30000000 | sz+5;	// DMAref
			// this obviously only works with 32 bit pointers, but it's only needed on the PS2 anyway
			*xferchain++ = (uint32)(uintptr)p - 0x50;
			*xferchain++ = 0;		// VIF nop
			*xferchain++ = 0x50000000 | sz+5;	// VIF DIRECT 2 qwords
		}
	}
	raster->originalPixels = raster->pixels;
	raster->originalStride = raster->stride;
	if(ras->flags & Ps2Raster::NEWSTYLE)
		raster->pixels = ((Ps2Raster::PixelPtr*)raster->pixels)->pixels + 0x50;
	return raster;
}

Raster*
rasterCreate(Raster *raster)
{
	if(!getRasterFormat(raster))
		return nil;

	// init raster
	raster->pixels = nil;
	raster->palette = nil;
	raster->originalWidth = raster->width;
	raster->originalHeight = raster->height;
	raster->originalPixels = raster->pixels;
	if(raster->width == 0 || raster->height == 0){
		raster->flags = Raster::DONTALLOCATE;
		raster->stride = 0;
		raster->originalStride = 0;
		return raster;
	}

	switch(raster->type){
	case Raster::NORMAL:
	case Raster::TEXTURE:
		return rasterCreateTexture(raster);
	case Raster::ZBUFFER:
		// TODO. only RW_PS2
		// get info from video mode
		raster->flags = Raster::DONTALLOCATE;
		return raster;
	case Raster::CAMERA:
		// TODO. only RW_PS2
		// get info from video mode
		raster->flags = Raster::DONTALLOCATE;
		return raster;
	case Raster::CAMERATEXTURE:
		// TODO. only RW_PS2
		// check width/height and fall through to texture
		return nil;
	}
	return nil;
}

static uint32
swizzle(uint32 x, uint32 y, uint32 logw)
{
#define X(n) ((x>>(n))&1)
#define Y(n) ((y>>(n))&1)

	uint32 nx, ny, n;
	x ^= (Y(1)^Y(2))<<2;
	nx = (x&7) | ((x>>1)&~7);
	ny = (y&1) | ((y>>1)&~1);
	n = Y(1) | X(3)<<1;
	return n | nx<<2 | ny<<(logw-1+2);
}

void
unswizzleRaster(Raster *raster)
{
	uint8 tmpbuf[1024*4];	// 1024x4px, maximum possible width
	uint32 mask;
	int32 x, y, w, h;
	int32 i;
	int32 logw;
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	uint8 *px;

	if((raster->format & (Raster::PAL4|Raster::PAL8)) == 0)
		return;

	int minw, minh;
	transferMinSize(raster->format & Raster::PAL4 ? PSMT4 : PSMT8, natras->flags, &minw, &minh);
	w = max(raster->width, minw);
	h = max(raster->height, minh);
	px = raster->pixels;
	logw = 0;
	for(i = 1; i < w; i *= 2) logw++;
	mask = (1<<(logw+2))-1;

	if(raster->format & Raster::PAL4 && natras->flags & Ps2Raster::SWIZZLED4){
		for(y = 0; y < h; y += 4){
			memcpy(tmpbuf, &px[y<<(logw-1)], 2*w);
			for(i = 0; i < 4; i++)
				for(x = 0; x < w; x++){
					uint32 a = ((y+i)<<logw)+x;
					uint32 s = swizzle(x, y+i, logw)&mask;
					uint8 c = s & 1 ? tmpbuf[s>>1] >> 4 : tmpbuf[s>>1] & 0xF;
					px[a>>1] = a & 1 ? (px[a>>1]&0xF) | c<<4 : (px[a>>1]&0xF0) | c;
				}
		}
	}else if(raster->format & Raster::PAL8 && natras->flags & Ps2Raster::SWIZZLED8){
		for(y = 0; y < h; y += 4){
			memcpy(tmpbuf, &px[y<<logw], 4*w);
			for(i = 0; i < 4; i++)
				for(x = 0; x < w; x++){
					uint32 a = ((y+i)<<logw)+x;
					uint32 s = swizzle(x, y+i, logw)&mask;
					px[a] = tmpbuf[s];
				}
		}
	}
}

void
swizzleRaster(Raster *raster)
{
	uint8 tmpbuf[1024*4];	// 1024x4px, maximum possible width
	uint32 mask;
	int32 x, y, w, h;
	int32 i;
	int32 logw;
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	uint8 *px;

	if((raster->format & (Raster::PAL4|Raster::PAL8)) == 0)
		return;

	int minw, minh;
	transferMinSize(raster->format & Raster::PAL4 ? PSMT4 : PSMT8, natras->flags, &minw, &minh);
	w = max(raster->width, minw);
	h = max(raster->height, minh);
	px = raster->pixels;
	logw = 0;
	for(i = 1; i < raster->width; i *= 2) logw++;
	mask = (1<<(logw+2))-1;

	if(raster->format & Raster::PAL4 && natras->flags & Ps2Raster::SWIZZLED4){
		for(y = 0; y < h; y += 4){
			for(i = 0; i < 4; i++)
				for(x = 0; x < w; x++){
					uint32 a = ((y+i)<<logw)+x;
					uint32 s = swizzle(x, y+i, logw)&mask;
					uint8 c = a & 1 ? px[a>>1] >> 4 : px[a>>1] & 0xF;
					tmpbuf[s>>1] = s & 1 ? (tmpbuf[s>>1]&0xF) | c<<4 : (tmpbuf[s>>1]&0xF0) | c;
				}
			memcpy(&px[y<<(logw-1)], tmpbuf, 2*w);
		}
	}else if(raster->format & Raster::PAL8 && natras->flags & Ps2Raster::SWIZZLED8){
		for(y = 0; y < h; y += 4){
			for(i = 0; i < 4; i++)
				for(x = 0; x < w; x++){
					uint32 a = ((y+i)<<logw)+x;
					uint32 s = swizzle(x, y+i, logw)&mask;
					tmpbuf[s] = px[a];
				}
			memcpy(&px[y<<logw], tmpbuf, 4*w);
		}
	}
}

uint8*
rasterLock(Raster *raster, int32 level, int32 lockMode)
{
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	assert(raster->depth != 24);

	if(level > 0){
		int32 minw, minh;
		int32 mipw, miph;
		transferMinSize(raster->format & Raster::PAL4 ? PSMT4 : PSMT8, natras->flags, &minw, &minh);
		while(level--){
			mipw = max(raster->width, minw);
			miph = max(raster->height, minh);
			raster->pixels += ALIGN16(mipw*miph*raster->depth/8) + 0x50;
			raster->width /= 2;
			raster->height /= 2;
		}
	}

	if((lockMode & Raster::LOCKNOFETCH) == 0)
		unswizzleRaster(raster);
	if(lockMode & Raster::LOCKREAD) raster->privateFlags |= Raster::PRIVATELOCK_READ;
	if(lockMode & Raster::LOCKWRITE) raster->privateFlags |= Raster::PRIVATELOCK_WRITE;
	return raster->pixels;
}

void
rasterUnlock(Raster *raster, int32 level)
{
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	if(raster->format & (Raster::PAL4 | Raster::PAL8))
		swizzleRaster(raster);

	raster->width = raster->originalWidth;
	raster->height = raster->originalHeight;
	raster->pixels = raster->originalPixels;
	raster->stride = raster->originalStride;
	if(natras->flags & Ps2Raster::NEWSTYLE)
		raster->pixels = ((Ps2Raster::PixelPtr*)raster->pixels)->pixels + 0x50;

	raster->privateFlags &= ~(Raster::PRIVATELOCK_READ|Raster::PRIVATELOCK_WRITE);
	// TODO: generate mipmaps
}

static void
convertCSM1_16(uint32 *pal)
{
	int i, j;
	uint32 tmp;
	for(i = 0; i < 256; i++)
		// palette index bits 0x08 and 0x10 are flipped
		if((i & 0x18) == 0x10){
			j = i ^ 0x18;
			tmp = pal[i];
			pal[i] = pal[j];
			pal[j] = tmp;
		}
}

static void
convertCSM1_32(uint32 *pal)
{
	int i, j;
	uint32 tmp;
	for(i = 0; i < 256; i++)
		// palette index bits 0x08 and 0x10 are flipped
		if((i & 0x18) == 0x10){
			j = i ^ 0x18;
			tmp = pal[i];
			pal[i] = pal[j];
			pal[j] = tmp;
		}
}

static void
convertPalette(Raster *raster)
{
	if(raster->format & Raster::PAL8){
		if((raster->format & 0xF00) == Raster::C8888)
			convertCSM1_32((uint32*)raster->palette);
		else if((raster->format & 0xF00) == Raster::C8888)
			convertCSM1_16((uint32*)raster->palette);
	}
}

// NB: RW doesn't convert the palette when locking/unlocking
uint8*
rasterLockPalette(Raster *raster, int32 lockMode)
{
	if((raster->format & (Raster::PAL4 | Raster::PAL8)) == 0)
		return nil;
	if((lockMode & Raster::LOCKNOFETCH) == 0)
		convertPalette(raster);
	if(lockMode & Raster::LOCKREAD) raster->privateFlags |= Raster::PRIVATELOCK_READ_PALETTE;
	if(lockMode & Raster::LOCKWRITE) raster->privateFlags |= Raster::PRIVATELOCK_WRITE_PALETTE;
	return raster->palette;
}

void
rasterUnlockPalette(Raster *raster)
{
	if(raster->format & (Raster::PAL4 | Raster::PAL8))
		convertPalette(raster);
	raster->privateFlags &= ~(Raster::PRIVATELOCK_READ_PALETTE|Raster::PRIVATELOCK_WRITE_PALETTE);
}

// Almost the same as d3d9 and gl3 function
bool32
imageFindRasterFormat(Image *img, int32 type,
	int32 *pWidth, int32 *pHeight, int32 *pDepth, int32 *pFormat)
{
	int32 width, height, depth, format;

	assert((type&0xF) == Raster::TEXTURE);

	for(width = 1; width < img->width; width <<= 1);
	for(height = 1; height < img->height; height <<= 1);

	depth = img->depth;

	switch(depth){
	case 32:
	case 24:
		// C888 24 bit is unsafe
		format = Raster::C8888;
		depth = 32;
		break;
	case 16:
		format = Raster::C1555;
		break;
	case 8:
		format = Raster::PAL8 | Raster::C8888;
		break;
	case 4:
		format = Raster::PAL4 | Raster::C8888;
		break;
	default:
		RWERROR((ERR_INVRASTER));
		return 0;
	}

	format |= type;

	*pWidth = width;
	*pHeight = height;
	*pDepth = depth;
	*pFormat = format;

	return 1;
}

bool32
rasterFromImage(Raster *raster, Image *image)
{
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);

	int32 pallength = 0;
	switch(image->depth){
	case 24:
	case 32:
		if(raster->format != Raster::C8888 &&
		   raster->format != Raster::C888)	// unsafe already
			goto err;
		break;
	case 16:
		if(raster->format != Raster::C1555) goto err;
		break;
	case 8:
		if(raster->format != (Raster::PAL8 | Raster::C8888)) goto err;
		pallength = 256;
		break;
	case 4:
		if(raster->format != (Raster::PAL4 | Raster::C8888)) goto err;
		pallength = 16;
		break;
	default:
	err:
		RWERROR((ERR_INVRASTER));
		return 0;
	}

	// unsafe
	if((raster->format&0xF00) == Raster::C888){
		RWERROR((ERR_INVRASTER));
		return 0;
	}

	uint8 *in, *out;
	if(image->depth <= 8){
		in = image->palette;
		out = raster->lockPalette(Raster::LOCKWRITE|Raster::LOCKNOFETCH);
		memcpy(out, in, 4*pallength);
		for(int32 i = 0; i < pallength; i++){
			out[3] = out[3]*128/255;
			out += 4;
		}
		raster->unlockPalette();
	}

	int minw, minh;
	int tw;
	transferMinSize(image->depth == 4 ? PSMT4 : PSMT8, natras->flags, &minw, &minh);
	tw = max(image->width, minw);
	uint8 *src = image->pixels;
	out = raster->lock(0, Raster::LOCKWRITE|Raster::LOCKNOFETCH);
	if(image->depth == 4){
		compressPal4(out, tw/2, src, image->stride, image->width, image->height);
	}else if(image->depth == 8){
		copyPal8(out, tw, src, image->stride, image->width, image->height);
	}else{
		for(int32 y = 0; y < image->height; y++){
			in = src;
			for(int32 x = 0; x < image->width; x++){
				switch(image->depth){
				case 16:
					conv_ARGB1555_from_ABGR1555(out, in);
					out += 2;
					break;
				case 24:
					out[0] = in[0];
					out[1] = in[1];
					out[2] = in[2];
					out[3] = 0x80;
					out += 4;
					break;
				case 32:
					out[0] = in[0];
					out[1] = in[1];
					out[2] = in[2];
					out[3] = in[3]*128/255;
					out += 4;
					break;
				}
				in += image->bpp;
			}
			src += image->stride;
		}
	}
	raster->unlock(0);
	return 1;
}

Image*
rasterToImage(Raster *raster)
{
	Image *image;
	int depth;
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);

	int32 rasterFormat = raster->format & 0xF00;
	switch(rasterFormat){
	case Raster::C1555:
		depth = 16;
		break;
	case Raster::C8888:
		depth = 32;
		break;
	case Raster::C888:
		depth = 24;
		break;
	case Raster::C555:
		depth = 16;
		break;

	default:
	case Raster::C565:
	case Raster::C4444:
	case Raster::LUM8:
		assert(0 && "unsupported ps2 raster format");
	}
	int32 pallength = 0;
	if((raster->format & Raster::PAL4) == Raster::PAL4){
		depth = 4;
		pallength = 16;
	}else if((raster->format & Raster::PAL8) == Raster::PAL8){
		depth = 8;
		pallength = 256;
	}

	uint8 *in, *out;
	image = Image::create(raster->width, raster->height, depth);
	image->allocate();

	if(pallength){
		out = image->palette;
		in = raster->lockPalette(Raster::LOCKREAD);
		if(rasterFormat == Raster::C8888){
			memcpy(out, in, pallength*4);
			for(int32 i = 0; i < pallength; i++){
				out[3] = out[3]*255/128;
				out += 4;
			}
		}else
			memcpy(out, in, pallength*2);
		raster->unlockPalette();
	}

	int minw, minh;
	int tw;
	transferMinSize(depth == 4 ? PSMT4 : PSMT8, natras->flags, &minw, &minh);
	tw = max(raster->width, minw);
	uint8 *dst = image->pixels;
	in = raster->lock(0, Raster::LOCKREAD);
	if(depth == 4){
		expandPal4(dst, image->stride, in, tw/2, raster->width, raster->height);
	}else if(depth == 8){
		copyPal8(dst, image->stride, in, tw, raster->width, raster->height);
	}else{
		for(int32 y = 0; y < image->height; y++){
			out = dst;
			for(int32 x = 0; x < image->width; x++){
				switch(raster->format & 0xF00){
				case Raster::C8888:
					out[0] = in[0];
					out[1] = in[1];
					out[2] = in[2];
					out[3] = in[3]*255/128;
					in += 4;
					break;
				case Raster::C888:
					out[0] = in[0];
					out[1] = in[1];
					out[2] = in[2];
					in += 4;
					break;
				case Raster::C1555:
					conv_ARGB1555_from_ABGR1555(out, in);
					in += 2;
					break;
				case Raster::C555:
					conv_ARGB1555_from_ABGR1555(out, in);
					out[1] |= 0x80;
					in += 2;
					break;
				default:
					assert(0 && "unknown ps2 raster format");
					break;
				}
				out += image->bpp;
			}
			dst += image->stride;
		}
	}
	raster->unlock(0);

	return image;
}

int32
rasterNumLevels(Raster *raster)
{
	Ps2Raster *ras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	if(raster->pixels == nil) return 0;
	if(raster->format & Raster::MIPMAP)
		return MAXLEVEL(ras)+1;
	return 1;
}

static void*
createNativeRaster(void *object, int32 offset, int32)
{
	Ps2Raster *raster = PLUGINOFFSET(Ps2Raster, object, offset);
	raster->tex0 = 0;
	raster->paletteBase = 0;
	raster->kl = defaultMipMapKL;
	raster->tex1low = 0;
	raster->unk2 = 0;
	raster->miptbp1 = 0;
	raster->miptbp2 = 0;
	raster->pixelSize = 0;
	raster->paletteSize = 0;
	raster->totalSize = 0;
	raster->flags = 0;

	raster->dataSize = 0;
	raster->data = nil;
	return object;
}

static void*
destroyNativeRaster(void *object, int32 offset, int32)
{
	Ps2Raster *raster = PLUGINOFFSET(Ps2Raster, object, offset);
	freealign(raster->data);
	return object;
}

static void*
copyNativeRaster(void *dst, void *src, int32 offset, int32)
{
	Ps2Raster *dstraster = PLUGINOFFSET(Ps2Raster, dst, offset);
	Ps2Raster *srcraster = PLUGINOFFSET(Ps2Raster, src, offset);
	*dstraster = *srcraster;
	return dst;
}

static Stream*
readMipmap(Stream *stream, int32, void *object, int32 offset, int32)
{
	uint16 val = stream->readI32();
	Texture *tex = (Texture*)object;
	if(tex->raster == nil)
		return stream;
	Ps2Raster *raster = PLUGINOFFSET(Ps2Raster, tex->raster, offset);
	raster->kl = val;
	return stream;
}

static Stream*
writeMipmap(Stream *stream, int32, void *object, int32 offset, int32)
{
	Texture *tex = (Texture*)object;
	if(tex->raster){
		stream->writeI32(defaultMipMapKL);
		return stream;
	}
	Ps2Raster *raster = PLUGINOFFSET(Ps2Raster, tex->raster, offset);
	stream->writeI32(raster->kl);
	return stream;
}

static int32
getSizeMipmap(void*, int32, int32)
{
	return rw::platform == PLATFORM_PS2 ? 4 : 0;
}

void
registerNativeRaster(void)
{
	nativeRasterOffset = Raster::registerPlugin(sizeof(Ps2Raster),
	                                            ID_RASTERPS2,
	                                            createNativeRaster,
	                                            destroyNativeRaster,
	                                            copyNativeRaster);

	Texture::registerPlugin(0, ID_SKYMIPMAP, nil, nil, nil);
	Texture::registerPluginStream(ID_SKYMIPMAP, readMipmap, writeMipmap, getSizeMipmap);
}

void
printTEX0(uint64 tex0)
{
	printf("%016lX ", tex0);
	uint32 tbp0 = tex0 & 0x3FFF; tex0 >>= 14;
	uint32 tbw = tex0 & 0x3F; tex0 >>= 6;
	uint32 psm = tex0 & 0x3F; tex0 >>= 6;
	uint32 tw = tex0 & 0xF; tex0 >>= 4;
	uint32 th = tex0 & 0xF; tex0 >>= 4;
	uint32 tcc = tex0 & 0x1; tex0 >>= 1;
	uint32 tfx = tex0 & 0x3; tex0 >>= 2;
	uint32 cbp = tex0 & 0x3FFF; tex0 >>= 14;
	uint32 cpsm = tex0 & 0xF; tex0 >>= 4;
	uint32 csm = tex0 & 0x1; tex0 >>= 1;
	uint32 csa = tex0 & 0x1F; tex0 >>= 5;
	uint32 cld = tex0 & 0x7;
	printf("TBP0:%4X TBW:%2X PSM:%2X TW:%X TH:%X TCC:%X TFX:%X CBP:%4X CPSM:%X CSM:%X CSA:%2X CLD:%X\n",
		tbp0, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld);
}

void
printTEX1(uint64 tex1)
{
	printf("%016lX ", tex1);
	uint32 lcm = tex1 & 0x1; tex1 >>= 2;
	uint32 mxl = tex1 & 0x7; tex1 >>= 3;
	uint32 mmag = tex1 & 0x1; tex1 >>= 1;
	uint32 mmin = tex1 & 0x7; tex1 >>= 3;
	uint32 mtba = tex1 & 0x1; tex1 >>= 10;
	uint32 l = tex1 & 0x3; tex1 >>= 13;
	uint32 k = tex1 & 0xFFF;
	printf("LCM:%X MXL:%X MMAG:%X MMIN:%X MTBA:%X L:%X K:%X\n",
		lcm, mxl, mmag, mmin, mtba, l, k);
}

void
calcTEX1(Raster *raster, uint64 *tex1, int32 filter)
{
	enum {
		NEAREST = 0,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_NEAREST,
		LINEAR_MIPMAP_LINEAR,
	};
	Ps2Raster *natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	uint64 t1 = natras->tex1low;
	uint64 k = natras->kl & 0xFFF;
	uint64 l = (natras->kl >> 12) & 0x3;
	t1 |= k << 32;
	t1 |= l << 19;
	switch(filter){
	case Texture::NEAREST:
		t1 |= (NEAREST << 5) |
		      (NEAREST << 6);
		break;
	case Texture::LINEAR:
		t1 |= (LINEAR << 5) |
		      (LINEAR << 6);
		break;
	case Texture::MIPNEAREST:
		t1 |= (NEAREST << 5) |
		      (NEAREST_MIPMAP_NEAREST << 6);
		break;
	case Texture::MIPLINEAR:
		t1 |= (LINEAR << 5) |
		      (LINEAR_MIPMAP_NEAREST << 6);
		break;
	case Texture::LINEARMIPNEAREST:
		t1 |= (NEAREST << 5) |
		      (NEAREST_MIPMAP_LINEAR << 6);
		break;
	case Texture::LINEARMIPLINEAR:
		t1 |= (LINEAR << 5) |
		      (LINEAR_MIPMAP_LINEAR << 6);
		break;
	}
	*tex1 = t1;
}

struct StreamRasterExt
{
	int32 width;
	int32 height;
	int32 depth;
	uint16 rasterFormat;
	int16  version;
	uint64 tex0;
	uint32 paletteOffset;
	uint32 tex1low;
	uint64 miptbp1;
	uint64 miptbp2;
	uint32 pixelSize;
	uint32 paletteSize;
	uint32 totalSize;
	uint32 mipmapVal;
};

Texture*
readNativeTexture(Stream *stream)
{
	uint32 length, oldversion, version;
	uint32 fourcc;
	Raster *raster;
	Ps2Raster *natras;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		return nil;
	}
	fourcc = stream->readU32();
	if(fourcc != FOURCC_PS2){
		RWERROR((ERR_PLATFORM, fourcc));
		return nil;
	}
	Texture *tex = Texture::create(nil);
	if(tex == nil)
		return nil;

	// Texture
	tex->filterAddressing = stream->readU32();
	if(!findChunk(stream, ID_STRING, &length, nil)){
		RWERROR((ERR_CHUNK, "STRING"));
		goto fail;
	}
	stream->read8(tex->name, length);
	if(!findChunk(stream, ID_STRING, &length, nil)){
		RWERROR((ERR_CHUNK, "STRING"));
		goto fail;
	}
	stream->read8(tex->mask, length);

	// Raster
	StreamRasterExt streamExt;
	oldversion = rw::version;
	if(!findChunk(stream, ID_STRUCT, nil, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		goto fail;
	}
	if(!findChunk(stream, ID_STRUCT, nil, &version)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		goto fail;
	}
	ASSERTLITTLE;
	stream->read8(&streamExt, 0x40);
/*
printf("%X %X %X %X %X %016llX %X %X %016llX %016llX %X %X %X %X\n",
streamExt.width,
streamExt.height,
streamExt.depth,
streamExt.rasterFormat,
streamExt.version,
streamExt.tex0,
streamExt.paletteOffset,
streamExt.tex1low,
streamExt.miptbp1,
streamExt.miptbp2,
streamExt.pixelSize,
streamExt.paletteSize,
streamExt.totalSize,
streamExt.mipmapVal);
*/
	noNewStyleRasters = streamExt.version < 2;
	rw::version = version;
	raster = Raster::create(streamExt.width, streamExt.height,
	                        streamExt.depth, streamExt.rasterFormat,
	                        PLATFORM_PS2);
	noNewStyleRasters = 0;
	rw::version = oldversion;
	tex->raster = raster;
	natras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
//printf("%X %X\n", natras->paletteBase, natras->tex1low);
//	printf("%08X%08X %08X%08X %08X%08X\n",
//	       (uint32)natras->tex0, (uint32)(natras->tex0>>32),
//	       (uint32)natras->miptbp1, (uint32)(natras->miptbp1>>32),
//	       (uint32)natras->miptbp2, (uint32)(natras->miptbp2>>32));
//	printTEX0(natras->tex0);
	uint64 tex1;
	calcTEX1(raster, &tex1, tex->filterAddressing & 0xF);
//	printTEX1(tex1);

	// TODO: GTA SA LD_OTB.txd loses here
	assert(natras->pixelSize >= streamExt.pixelSize);
	assert(natras->paletteSize >= streamExt.paletteSize);

//if(natras->tex0 != streamExt.tex0){
//printf("TEX0: %016llX\n      %016llX\n", natras->tex0, streamExt.tex0);
//printTEX0(natras->tex0);
//printTEX0(streamExt.tex0);
//fflush(stdout);
//}
//if(natras->tex1low != streamExt.tex1low)
//printf("TEX1: %08X\n      %08X\n", natras->tex1low, streamExt.tex1low);
//if(natras->miptbp1 != streamExt.miptbp1)
//printf("MIP1: %016llX\n      %016llX\n", natras->miptbp1, streamExt.miptbp1);
//if(natras->miptbp2 != streamExt.miptbp2)
//printf("MIP2: %016llX\n      %016llX\n", natras->miptbp2, streamExt.miptbp2);
//if(natras->paletteBase != streamExt.paletteOffset)
//printf("PAL: %08X\n     %08X\n", natras->paletteBase, streamExt.paletteOffset);
//if(natras->pixelSize != streamExt.pixelSize)
//printf("PXS: %08X\n     %08X\n", natras->pixelSize, streamExt.pixelSize);
//if(natras->paletteSize != streamExt.paletteSize)
//printf("PLS: %08X\n     %08X\n", natras->paletteSize, streamExt.paletteSize);
//if(natras->totalSize != streamExt.totalSize)
//printf("TSZ: %08X\n     %08X\n", natras->totalSize, streamExt.totalSize);

	// junk addresses, no need to store them
	streamExt.tex0 &= ~0x3FFFULL;
	streamExt.tex0 &= ~(0x3FFFULL << 37);

	assert(natras->tex0 == streamExt.tex0);
	natras->tex0 = streamExt.tex0;
	assert(natras->paletteBase == streamExt.paletteOffset);
	natras->paletteBase = streamExt.paletteOffset;
	assert(natras->tex1low == streamExt.tex1low);
	natras->tex1low = streamExt.tex1low;
	assert(natras->miptbp1 == streamExt.miptbp1);
	natras->miptbp1 = streamExt.miptbp1;
	assert(natras->miptbp2 == streamExt.miptbp2);
	natras->miptbp2 = streamExt.miptbp2;
	assert(natras->pixelSize == streamExt.pixelSize);
	natras->pixelSize = streamExt.pixelSize;
	assert(natras->paletteSize == streamExt.paletteSize);
	natras->paletteSize = streamExt.paletteSize;
	assert(natras->totalSize == streamExt.totalSize);
	natras->totalSize = streamExt.totalSize;
	natras->kl = streamExt.mipmapVal;
//printf("%X %X\n", natras->paletteBase, natras->tex1low);
//	printf("%08X%08X %08X%08X %08X%08X\n",
//	       (uint32)natras->tex0, (uint32)(natras->tex0>>32),
//	       (uint32)natras->miptbp1, (uint32)(natras->miptbp1>>32),
//	       (uint32)natras->miptbp2, (uint32)(natras->miptbp2>>32));
//	printTEX0(natras->tex0);
	calcTEX1(raster, &tex1, tex->filterAddressing & 0xF);
//	printTEX1(tex1);

	// this is weird stuff
	if(streamExt.version < 2){
		if(streamExt.version == 1){
			// Version 1 has swizzled 8 bit textures
			if(!(natras->flags & Ps2Raster::NEWSTYLE))
				natras->flags |= Ps2Raster::SWIZZLED8;
			else
				assert(0 && "can't happen");
		}else{
			// Version 0 has no swizzling at all
			if(!(natras->flags & Ps2Raster::NEWSTYLE))
				natras->flags &= ~Ps2Raster::SWIZZLED8;
			else
				assert(0 && "can't happen");
		}
	}

	if(!findChunk(stream, ID_STRUCT, &length, nil)){
		RWERROR((ERR_CHUNK, "STRUCT"));
		goto fail;
	}
	if(streamExt.version < 2){
		stream->read8(raster->pixels, length);
	}else{
		stream->read8(((Ps2Raster::PixelPtr*)raster->originalPixels)->pixels, natras->pixelSize);
		stream->read8(raster->palette-0x50, natras->paletteSize);
	}
//printf("\n");
	return tex;

fail:
	tex->destroy();
	return nil;
}

void
writeNativeTexture(Texture *tex, Stream *stream)
{
	Raster *raster = tex->raster;
	Ps2Raster *ras = PLUGINOFFSET(Ps2Raster, raster, nativeRasterOffset);
	writeChunkHeader(stream, ID_STRUCT, 8);
	stream->writeU32(FOURCC_PS2);
	stream->writeU32(tex->filterAddressing);
	int32 len = strlen(tex->name)+4 & ~3;
	writeChunkHeader(stream, ID_STRING, len);
	stream->write8(tex->name, len);
	len = strlen(tex->mask)+4 & ~3;
	writeChunkHeader(stream, ID_STRING, len);
	stream->write8(tex->mask, len);

	int32 sz = ras->pixelSize + ras->paletteSize;
	writeChunkHeader(stream, ID_STRUCT, 12 + 64 + 12 + sz);
	writeChunkHeader(stream, ID_STRUCT, 64);
	StreamRasterExt streamExt;
	streamExt.width = raster->width;
	streamExt.height = raster->height;
	streamExt.depth = raster->depth;
	streamExt.rasterFormat = raster->format | raster->type;
	streamExt.version = 0;
	if(ras->flags == Ps2Raster::SWIZZLED8 && raster->depth == 8)
		streamExt.version = 1;
	if(ras->flags & Ps2Raster::NEWSTYLE)
		streamExt.version = 2;
	streamExt.tex0 = ras->tex0;
	streamExt.paletteOffset = ras->paletteBase;
	streamExt.tex1low = ras->tex1low;
	streamExt.miptbp1 = ras->miptbp1;
	streamExt.miptbp2 = ras->miptbp2;
	streamExt.pixelSize = ras->pixelSize;
	streamExt.paletteSize = ras->paletteSize;
	streamExt.totalSize = ras->totalSize;
	streamExt.mipmapVal = ras->kl;
	ASSERTLITTLE;
	stream->write8(&streamExt, 64);

	writeChunkHeader(stream, ID_STRUCT, sz);
	if(streamExt.version < 2){
		stream->write8(raster->pixels, sz);
	}else{
		stream->write8(((Ps2Raster::PixelPtr*)raster->originalPixels)->pixels, ras->pixelSize);
		stream->write8(raster->palette-0x50, ras->paletteSize);
	}
}

uint32
getSizeNativeTexture(Texture *tex)
{
	uint32 size = 12 + 8;
	size += 12 + strlen(tex->name)+4 & ~3;
	size += 12 + strlen(tex->mask)+4 & ~3;
	size += 12;
	size += 12 + 64;
	Ps2Raster *ras = PLUGINOFFSET(Ps2Raster, tex->raster, nativeRasterOffset);
	size += 12 + ras->pixelSize + ras->paletteSize;
	return size;
}

}
}
