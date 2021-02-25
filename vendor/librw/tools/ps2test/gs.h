#define GS_NONINTERLACED	0
#define GS_INTERLACED		1

#define GS_NTSC		2
#define GS_PAL		3
#define GS_VESA1A	0x1a
#define GS_VESA1B	0x1b
#define GS_VESA1C	0x1c
#define GS_VESA1D	0x1d
#define GS_VESA2A	0x2a
#define GS_VESA2B	0x2b
#define GS_VESA2C	0x2c
#define GS_VESA2D	0x2d
#define GS_VESA2E	0x2e
#define GS_VESA3B	0x3b
#define GS_VESA3C	0x3c
#define GS_VESA3D	0x3d
#define GS_VESA3E	0x3e
#define GS_VESA4A	0x4a
#define GS_VESA4B	0x4b
#define GS_DTV480P	0x50

#define GS_FIELD		0
#define GS_FRAME		1

#define GS_PSMCT32		0
#define GS_PSMCT24		1
#define GS_PSMCT16		2
#define GS_PSMCT16S		10
#define GS_PS_GPU24		18

#define GS_PSMZ32		0
#define GS_PSMZ24		1
#define GS_PSMZ16		2
#define GS_PSMZ16S		10

#define GS_ZTST_NEVER		0
#define GS_ZTST_ALWAYS		1
#define GS_ZTST_GREATER		2
#define GS_ZTST_GEQUAL		3

#define GS_PRIM_POINT		0
#define GS_PRIM_LINE		1
#define GS_PRIM_LINE_STRIP	2
#define GS_PRIM_TRI		3
#define GS_PRIM_TRI_STRIP	4
#define GS_PRIM_TRI_FAN		5
#define GS_PRIM_SPRITE		6
#define GS_PRIM_NO_SPEC		7
#define GS_IIP_FLAT		0
#define GS_IIP_GOURAUD		1

/* GS general purpose registers */

#define GS_PRIM		0x00
#define GS_RGBAQ	0x01
#define GS_ST		0x02
#define GS_UV		0x03
#define GS_XYZF2	0x04
#define GS_XYZ2		0x05
#define GS_TEX0_1	0x06
#define GS_TEX0_2	0x07
#define GS_CLAMP_1	0x08
#define GS_CLAMP_2	0x09
#define GS_FOG		0x0a
#define GS_XYZF3	0x0c
#define GS_XYZ3		0x0d
#define GS_TEX1_1	0x14
#define GS_TEX1_2	0x15
#define GS_TEX2_1	0x16
#define GS_TEX2_2	0x17
#define GS_XYOFFSET_1	0x18
#define GS_XYOFFSET_2	0x19
#define GS_PRMODECONT	0x1a
#define GS_PRMODE	0x1b
#define GS_TEXCLUT	0x1c
#define GS_SCANMSK	0x22
#define GS_MIPTBP1_1	0x34
#define GS_MIPTBP1_2	0x35
#define GS_MIPTBP2_1	0x36
#define GS_MIPTBP2_2	0x37
#define GS_TEXA		0x3b
#define GS_FOGCOL	0x3d
#define GS_TEXFLUSH	0x3f
#define GS_SCISSOR_1	0x40
#define GS_SCISSOR_2	0x41
#define GS_ALPHA_1	0x42
#define GS_ALPHA_2	0x43
#define GS_DIMX		0x44
#define GS_DTHE		0x45
#define GS_COLCLAMP	0x46
#define GS_TEST_1	0x47
#define GS_TEST_2	0x48
#define GS_PABE		0x49
#define GS_FBA_1	0x4a
#define GS_FBA_2	0x4b
#define GS_FRAME_1	0x4c
#define GS_FRAME_2	0x4d
#define GS_ZBUF_1	0x4e
#define GS_ZBUF_2	0x4f
#define GS_BITBLTBUF	0x50
#define GS_TRXPOS	0x51
#define GS_TRXREG	0x52
#define GS_TRXDIR	0x53
#define GS_HWREG	0x54
#define GS_SIGNAL	0x60
#define GS_FINISH	0x61
#define GS_LABEL	0x62

typedef union
{
	struct {
		uint64 EN1 : 1;
		uint64 EN2 : 1;
		uint64 CRTMD : 3;
		uint64 MMOD : 1;
		uint64 AMOD : 1;
		uint64 SLBG : 1;
		uint64 ALP : 8;
	} f;
	uint64 d;
} GsPmode;

#define GS_MAKE_PMODE(EN1,EN2,MMOD,AMOD,SLBG,ALP) \
	(BIT64(EN1,0) | BIT64(EN2,1) | BIT64(1,2) | \
	 BIT64(MMOD,5) | BIT64(AMOD,6) | BIT64(SLBG,7) | BIT64(ALP,8))

typedef union
{
	struct {
		uint64 INT : 1;
		uint64 FFMD : 1;
		uint64 DPMS : 2;
	} f;
	uint64 d;
} GsSmode2;

#define GS_MAKE_SMODE2(INT,FFMD,DPMS) \
	(BIT64(INT,0) | BIT64(FFMD,1) | BIT64(DPMS,2))

typedef union
{
	struct {
		uint64 FBP : 9;
		uint64 FBW : 6;
		uint64 PSM : 5;
		uint64 : 12;
		uint64 DBX : 11;
		uint64 DBY : 11;
	} f;
	uint64 d;
} GsDispfb;

#define GS_MAKE_DISPFB(FBP,FBW,PSM,DBX,DBY) \
	(BIT64(FBP,0) | BIT64(FBW,9) | BIT64(PSM,15) | \
	 BIT64(DBX,32) | BIT64(DBY,43))

typedef union
{
	struct {
		uint64 DX : 12;
		uint64 DY : 11;
		uint64 MAGH : 4;
		uint64 MAGV : 2;
		uint64 : 3;
		uint64 DW : 12;
		uint64 DH : 11;
	} f;
	uint64 d;
} GsDisplay;

#define GS_MAKE_DISPLAY(DX,DY,MAGH,MAGV,DW,DH) \
	(BIT64(DX,0) | BIT64(DY,12) | BIT64(MAGH,23) | \
	 BIT64(MAGV,27) | BIT64(DW,32) | BIT64(DH,44))

typedef union
{
	struct {
		uint64 EXBP : 14;
		uint64 EXBW : 6;
		uint64 FBIN : 2;
		uint64 WFFMD : 1;
		uint64 EMODA : 2;
		uint64 EMODC : 2;
		uint64 : 5;
		uint64 WDX : 11;
		uint64 WDY : 11;
	} f;
	uint64 d;
} GsExtbuf;

#define GS_MAKE_EXTBUF(EXBP,EXBW,FBIN,WFFMD,EMODA,EMODC,WDX,WDY) \
	(BIT64(EXBP,0) | BIT64(EXBW,14) | BIT64(FBIN,20) | \
	 BIT64(WFFMD,22) | BIT64(EMODA,23) | BIT64(EMODC,25) | \
	 BIT64(WDX,32) | BIT64(WDY,43))

typedef union
{
	struct {
		uint64 SX : 12;
		uint64 SY : 11;
		uint64 SMPH : 4;
		uint64 SMPV : 2;
		uint64 : 3;
		uint64 WW : 12;
		uint64 WH : 11;
	} f;
	uint64 d;
} GsExtdata;

#define GS_MAKE_EXTDATA(SX,SY,SMPH,SMPV,WW,WH) \
	(BIT64(SX,0) | BIT64(SY,12) | BIT64(SMPH,23) | \
	 BIT64(SMPV,27) | BIT64(WW,32) | BIT64(WH,44))

typedef union
{
	struct {
		uint64 WRITE : 1;
	} f;
	uint64 d;
} GsExtwrite;

typedef union
{
	struct {
		uint64 R : 8;
		uint64 G : 8;
		uint64 B : 8;
	} f;
	uint64 d;
} GsBgcolor;

#define GS_MAKE_BGCOLOR(R,G,B) \
	(BIT64(R,0) | BIT64(G,8) | BIT64(B,16))

typedef union
{
	struct {
		uint64 SIGNAL : 1;
		uint64 FINISH : 1;
		uint64 HSINT : 1;
		uint64 VSINT : 1;
		uint64 EDWINT : 1;
		uint64 : 3;
		uint64 FLUSH : 1;
		uint64 RESET : 1;
		uint64 : 2;
		uint64 NFIELD : 1;
		uint64 FIELD : 1;
		uint64 FIFO : 2;
		uint64 REV : 8;
		uint64 ID : 8;
	} f;
	uint64 d;
} GsCsr;

#define GS_CSR_SIGNAL_O	0
#define GS_CSR_FINISH_O	1
#define GS_CSR_HSINT_O	2
#define GS_CSR_VSINT_O	3
#define GS_CSR_EDWINT_O	4
#define GS_CSR_FLUSH_O	8
#define GS_CSR_RESET_O	9
#define GS_CSR_NFIELD_O	12
#define GS_CSR_FIELD_O	13
#define GS_CSR_FIFO_O	14
#define GS_CSR_REV_O	16
#define GS_CSR_ID_O	24

typedef union
{
	struct {
		uint64 : 8;
		uint64 SIGMSK : 1;
		uint64 FINISHMSK : 1;
		uint64 HSMSKMSK : 1;
		uint64 VSMSKMSK : 1;
		uint64 EDWMSKMSK : 1;
	} f;
	uint64 d;
} GsImr;

typedef union
{
	struct {
		uint64 DIR : 1;
	} f;
	uint64 d;
} GsBusdir;

typedef union
{
	struct {
		uint64 SIGID : 32;
		uint64 LBLID : 32;
	} f;
	uint64 d;
} GsSiglblid;


typedef union
{
	struct {
		uint64 FBP : 9;
		uint64 : 7;
		uint64 FBW : 6;
		uint64 : 2;
		uint64 PSM : 6;
		uint64 : 2;
		uint64 FBMSK : 32;
	} f;
	uint64 d;
} GsFrame;

#define GS_MAKE_FRAME(FBP,FBW,PSM,FBMASK) \
	(BIT64(FBP,0) | BIT64(FBW,16) | BIT64(PSM,24) | BIT64(FBMASK,32))

typedef union
{
	struct {
		uint64 ZBP : 9;
		uint64 : 15;
		uint64 PSM : 4;
		uint64 : 4;
		uint64 ZMSDK : 1;
	} f;
	uint64 d;
} GsZbuf;

#define GS_MAKE_ZBUF(ZBP,PSM,ZMSK) \
	(BIT64(ZBP,0) | BIT64(PSM,24) | BIT64(ZMSK,32))

typedef union
{
	struct {
		uint64 OFX : 16;
		uint64 : 16;
		uint64 OFY : 16;
	} f;
	uint64 d;
} GsXyOffset;

#define GS_MAKE_XYOFFSET(OFX,OFY) \
	(BIT64(OFX,0) | BIT64(OFY,32))

typedef union
{
	struct {
		uint64 SCAX0 : 11;
		uint64 : 5;
		uint64 SCAX1 : 11;
		uint64 : 5;
		uint64 SCAY0 : 11;
		uint64 : 5;
		uint64 SCAY1 : 11;
	} f;
	uint64 d;
} GsScissor;

#define GS_MAKE_SCISSOR(SCAX0,SCAX1,SCAY0,SCAY1) \
	(BIT64(SCAX0,0) | BIT64(SCAX1,16) | BIT64(SCAY0,32) | BIT64(SCAY1,48))

#define GS_MAKE_TEST(ATE,ATST,AREF,AFAIL,DATE,DATM,ZTE,ZTST) \
	(BIT64(ATE,0) | BIT64(ATST,1) | BIT64(AREF,4) | BIT64(AFAIL,12) | \
	 BIT64(DATE,14) | BIT64(DATM,15) | BIT64(ZTE,16) | BIT64(ZTST,17))

#define GS_MAKE_PRIM(PRIM,IIP,TME,FGE,ABE,AA1,FST,CTXT,FIX) \
	(BIT64(PRIM,0) | BIT64(IIP,3) | BIT64(TME,4) | BIT64(FGE,5) | \
	 BIT64(ABE,6) | BIT64(AA1,7) | BIT64(FST,8) | BIT64(CTXT,9) | BIT64(FIX,10))

#define GS_MAKE_RGBAQ(R,G,B,A,Q) \
	(BIT64(R,0) | BIT64(G,8) | BIT64(B,16) | BIT64(A,24) | BIT64(Q,32))

#define GS_MAKE_XYZ(X,Y,Z) \
	(BIT64(X,0) | BIT64(Y,16) | BIT64(Z,32))

#define GIF_PACKED	0
#define GIF_REGLIST	1
#define GIF_IMAGE	2

#define GIF_MAKE_TAG(NLOOP,EOP,PRE,PRIM,FLG,NREG) \
	(BIT64(NLOOP,0) | BIT64(EOP,15) | BIT64(PRE,46) | \
	 BIT64(PRIM,47) | BIT64(FLG,58) | BIT64(NREG,60))

/* This is global and not tied to a user context because
 * it is set up by kernel functions and not really changed
 * afterwards. */
typedef struct GsCrtState GsCrtState;
struct GsCrtState
{
	short inter, mode, ff;
};
extern GsCrtState gsCrtState;

typedef struct GsDispCtx GsDispCtx;
struct GsDispCtx
{
	// two circuits
	GsPmode pmode;
	GsDispfb dispfb1;
	GsDispfb dispfb2;
	GsDisplay display1;
	GsDisplay display2;
	GsBgcolor bgcolor;
};

typedef struct GsDrawCtx GsDrawCtx;
struct GsDrawCtx
{
	//two contexts
	uint128 gifTag;
	GsFrame frame1;
	uint64  ad_frame1;
	GsFrame frame2;
	uint64  ad_frame2;
	GsZbuf  zbuf1;
	uint64  ad_zbuf1;
	GsZbuf  zbuf2;
	uint64  ad_zbuf2;
	GsXyOffset xyoffset1;
	uint64  ad_xyoffset1;
	GsXyOffset xyoffset2;
	uint64  ad_xyoffset2;
	GsScissor scissor1;
	uint64 ad_scissor1;
	GsScissor scissor2;
	uint64 ad_scissor2;
};

typedef struct GsCtx GsCtx;
struct GsCtx
{
	// display context; two buffers
	GsDispCtx disp[2];
	// draw context; two buffers
	GsDrawCtx draw[2];
};
