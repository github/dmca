#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <rw.h>
#include <args.h>

using namespace rw;

char *argv0;

void
usage(void)
{
	fprintf(stderr, "usage: %s in.ska [out.anm]\n", argv0);
	fprintf(stderr, "   or: %s in.anm [out.ska]\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[])
{
	rw::Engine::init();
	rw::registerHAnimPlugin();
	rw::Engine::open(nil);
	rw::Engine::start();

	ARGBEGIN{
	case 'v':
		sscanf(EARGF(usage()), "%x", &rw::version);
		break;
	default:
		usage();
	}ARGEND;

	if(argc < 1)
		usage();

	StreamFile stream;
	if(!stream.open(argv[0], "rb")){
		fprintf(stderr, "Error: couldn't open %s\n", argv[0]);
		return 1;
	}

	int32 firstword = stream.readU32();
	stream.seek(0, 0);
	Animation *anim = nil;
	if(firstword == ID_ANIMANIMATION){
		// it's an anm file
		if(findChunk(&stream, ID_ANIMANIMATION, nil, nil))
			anim = Animation::streamRead(&stream);
	}else{
		// it's a ska file
		anim = Animation::streamReadLegacy(&stream);
	}
	stream.close();

	if(anim == nil){
		fprintf(stderr, "Error: couldn't read anim file\n");
		return 1;
	}

	const char *file;
	if(argc > 1)
		file = argv[1];
	else if(firstword == ID_ANIMANIMATION)
		file = "out.ska";
	else
		file = "out.anm";
	if(!stream.open(file, "wb")){
		fprintf(stderr, "Error: couldn't open %s\n", file);
		return 1;
	}
	if(firstword == ID_ANIMANIMATION)
		anim->streamWriteLegacy(&stream);
	else
		anim->streamWrite(&stream);

	anim->destroy();

	return 0;
}
