#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <rw.h>

using namespace std;
using namespace rw;

const char *chunks[] = { "None", "Struct", "String", "Extension", "Unknown",
	"Camera", "Texture", "Material", "Material List", "Atomic Section",
	"Plane Section", "World", "Spline", "Matrix", "Frame List",
	"Geometry", "Clump", "Unknown", "Light", "Unicode String", "Atomic",
	"Texture Native", "Texture Dictionary", "Animation Database",
	"Image", "Skin Animation", "Geometry List", "Anim Animation",
	"Team", "Crowd", "Delta Morph Animation", "Right To Render",
	"MultiTexture Effect Native", "MultiTexture Effect Dictionary",
	"Team Dictionary", "Platform Independet Texture Dictionary",
	"Table of Contents", "Particle Standard Global Data", "AltPipe",
	"Platform Independet Peds", "Patch Mesh", "Chunk Group Start",
	"Chunk Group End", "UV Animation Dictionary", "Coll Tree"
};

/* From 0x0101 through 0x0135 */
const char *toolkitchunks0[] = { "Metrics PLG", "Spline PLG", "Stereo PLG",
	"VRML PLG", "Morph PLG", "PVS PLG", "Memory Leak PLG", "Animation PLG",
	"Gloss PLG", "Logo PLG", "Memory Info PLG", "Random PLG",
	"PNG Image PLG", "Bone PLG", "VRML Anim PLG", "Sky Mipmap Val",
	"MRM PLG", "LOD Atomic PLG", "ME PLG", "Lightmap PLG",
	"Refine PLG", "Skin PLG", "Label PLG", "Particles PLG", "GeomTX PLG",
	"Synth Core PLG", "STQPP PLG",
	"Part PP PLG", "Collision PLG", "HAnim PLG", "User Data PLG",
	"Material Effects PLG", "Particle System PLG", "Delta Morph PLG",
	"Patch PLG", "Team PLG", "Crowd PP PLG", "Mip Split PLG",
	"Anisotrophy PLG", "Not used", "GCN Material PLG", "Geometric PVS PLG",
	"XBOX Material PLG", "Multi Texture PLG", "Chain PLG", "Toon PLG",
	"PTank PLG", "Particle Standard PLG", "PDS PLG", "PrtAdv PLG",
	"Normal Map PLG", "ADC PLG", "UV Animation PLG"
};

/* From 0x0180 through 0x01c1 */
const char *toolkitchunks1[] = {
	"Character Set PLG", "NOHS World PLG", "Import Util PLG",
	"Slerp PLG", "Optim PLG", "TL World PLG", "Database PLG",
	"Raytrace PLG", "Ray PLG", "Library PLG",
	"Not used", "Not used", "Not used", "Not used", "Not used", "Not used",
	"2D PLG", "Tile Render PLG", "JPEG Image PLG", "TGA Image PLG",
	"GIF Image PLG", "Quat PLG", "Spline PVS PLG", "Mipmap PLG",
	"MipmapK PLG", "2D Font", "Intersection PLG", "TIFF Image PLG",
	"Pick PLG", "BMP Image PLG", "RAS Image PLG", "Skin FX PLG",
	"VCAT PLG", "2D Path", "2D Brush", "2D Object", "2D Shape", "2D Scene",
	"2D Pick Region", "2D Object String", "2D Animation PLG",
	"2D Animation",
	"Not used", "Not used", "Not used", "Not used", "Not used", "Not used",
	"2D Keyframe", "2D Maestro", "Barycentric",
	"Platform Independent Texture Dictionary TK", "TOC TK", "TPL TK",
	"AltPipe TK", "Animation TK", "Skin Split Tookit", "Compressed Key TK",
	"Geometry Conditioning PLG", "Wing PLG", "Generic Pipeline TK",
	"Lightmap Conversion TK", "Filesystem PLG", "Dictionary TK",
	"UV Animation Linear", "UV Animation Parameter"
};

const char *RSchunks[] = { "Unused 1", "Unused 2", "Extra Normals",
	"Pipeline Set", "Unused 5", "Unused 6", "Specular Material",
	"Unused 8", "2dfx", "Extra Colors", "Collision Model",
	"Unused 12", "Environment Material", "Breakable", "Node Name",
	"Unused 16"
};

const char*
getChunkName(uint32 id)
{
	switch(id){
	case 0x50E:
		return "Bin Mesh PLG";
	case 0x510:
		return "Native Data PLG";
	case 0x511:
		return "Vertex Format PLG";
	case 0xF21E:
		return "ZModeler Lock";
	}

	if(id <= 45)
		return chunks[id];
        else if(id <= 0x0253F2FF && id >= 0x0253F2F0)
                return RSchunks[id-0x0253F2F0];
        else if(id <= 0x0135 && id >= 0x0101)
                return toolkitchunks0[id-0x0101];
        else if(id <= 0x01C0 && id >= 0x0181)
                return toolkitchunks1[id-0x0181];
        else
                return "Unknown";
}

void
readchunk(StreamFile *s, ChunkHeaderInfo *h, int level)
{
	for(int i = 0; i < level; i++)
		printf("  ");
	const char *name = getChunkName(h->type);
	printf("%s (%x bytes @ 0x%x/0x%x) - [0x%x]\n",
		name, h->length, s->tell()-12, s->tell(), h->type);

	uint32 end = s->tell() + h->length;
	while(s->tell() < end){
		ChunkHeaderInfo nh;
		readChunkHeaderInfo(s, &nh);
		if(nh.version == h->version && nh.build == h->build){
			readchunk(s, &nh, level+1);
			if(h->type == 0x510)
				s->seek(end, 0);
		}else{
			s->seek(h->length-12);
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "usage: %s rwStreamFile\n", argv[0]);
		return 0;
	}
	StreamFile s;
	s.open(argv[1], "rb");

	ChunkHeaderInfo header, last;
	while(readChunkHeaderInfo(&s, &header)){
		if(header.type == 0)
			break;
		last = header;
		if(argc == 2)
			readchunk(&s, &header, 0);
	}

	printf("%x %x %x\n", last.version, last.build,
		libraryIDPack(last.version, last.build));

	s.close();
	return 0;
}
