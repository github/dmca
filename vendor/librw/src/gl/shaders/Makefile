all: header_vs.inc header_fs.inc im2d_gl.inc im3d_gl.inc default_vs_gl.inc simple_fs_gl.inc matfx_gl.inc skin_gl.inc

header_vs.inc: header.vert
	(echo 'const char *header_vert_src =';\
	 sed 's/..*/"&\\n"/' header.vert;\
	 echo ';') >header_vs.inc

header_fs.inc: header.frag
	(echo 'const char *header_frag_src =';\
	 sed 's/..*/"&\\n"/' header.frag;\
	 echo ';') >header_fs.inc

im2d_gl.inc: im2d.vert
	(echo 'const char *im2d_vert_src =';\
	 sed 's/..*/"&\\n"/' im2d.vert;\
	 echo ';') >im2d_gl.inc

im3d_gl.inc: im3d.vert
	(echo 'const char *im3d_vert_src =';\
	 sed 's/..*/"&\\n"/' im3d.vert;\
	 echo ';') >im3d_gl.inc

default_vs_gl.inc: default.vert
	(echo 'const char *default_vert_src =';\
	 sed 's/..*/"&\\n"/' default.vert;\
	 echo ';') >default_vs_gl.inc

simple_fs_gl.inc: simple.frag
	(echo 'const char *simple_frag_src =';\
	 sed 's/..*/"&\\n"/' simple.frag;\
	 echo ';') >simple_fs_gl.inc

matfx_gl.inc: matfx_env.frag matfx_env.vert
	(echo 'const char *matfx_env_vert_src =';\
	 sed 's/..*/"&\\n"/' matfx_env.vert;\
	 echo ';';\
	 echo 'const char *matfx_env_frag_src =';\
	 sed 's/..*/"&\\n"/' matfx_env.frag;\
	 echo ';') >matfx_gl.inc

skin_gl.inc: skin.vert
	(echo 'const char *skin_vert_src =';\
	 sed 's/..*/"&\\n"/' skin.vert;\
	 echo ';') >skin_gl.inc

