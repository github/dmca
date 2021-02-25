extern char *argv0;
#define USED(x) ((void)x)
#define SET(x) ((x)=0)

#define	ARGBEGIN	for((argv0||(argv0=*argv)),argv++,argc--;\
			    argv[0] && argv[0][0]=='-' && argv[0][1];\
			    argc--, argv++) {\
				char *_args, *_argt;\
				char _argc;\
				_args = &argv[0][1];\
				if(_args[0]=='-' && _args[1]==0){\
					argc--; argv++; break;\
				}\
				_argc = 0;\
				while(*_args && (_argc = *_args++))\
				switch(_argc)
#define	ARGEND		SET(_argt);USED(_argt);USED(_argc);USED(_args);}USED(argv);USED(argc);
#define	ARGF()		(_argt=_args, _args=(char*)"",\
				(*_argt? _argt: argv[1]? (argc--, *++argv): 0))
#define	EARGF(x)	(_argt=_args, _args=(char*)"",\
				(*_argt? _argt: argv[1]? (argc--, *++argv): ((x), abort(), (char*)0)))

#define	ARGC()		_argc

