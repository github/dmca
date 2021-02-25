namespace rw {

struct Charset
{
	struct Desc {
		int32 count;	// num of chars
		int32 tileWidth, tileHeight;	// chars in raster
		int32 width, height;	// of char
		int32 width_internal, height_internal;	// + border
	} desc;
	Raster *raster;

	static bool32 open(void);
	static void close(void);
	static Charset *create(const RGBA *foreground, const RGBA *background);
	void destroy(void);
	Charset *setColors(const RGBA *foreground, const RGBA *background);
	void print(const char *str, int32 x, int32 y, bool32 hideSpaces);
	void printBuffered(const char *str, int32 x, int32 y, bool32 hideSpaces);
	static void flushBuffer(void);
private:
	void printChar(int32 c, int32 x, int32 y);
};

}
