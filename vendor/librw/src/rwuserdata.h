namespace rw {

enum UserDataType
{
	USERDATANA = 0,
	USERDATAINT = 1,
	USERDATAFLOAT = 2,
	USERDATASTRING = 3
};

struct UserDataArray
{
	char *name;
	uint32 datatype;
	int32 numElements;
	void *data;

	int32 getInt(int32 n) { return ((int32*)this->data)[n]; }
	float32 getFloat(int32 n) { return ((float32*)this->data)[n]; }
	char *getString(int32 n) { return ((char**)this->data)[n]; }
	void setInt(int32 n, int32 i) { ((int32*)this->data)[n] = i; }
	void setFloat(int32 n, float32 f) { ((float32*)this->data)[n] = f; }
	void setString(int32 n, const char *s);

	static int32 geometryAdd(Geometry *g, const char *name, int32 datatype, int32 numElements);
	static void geometryRemove(Geometry *g, int32 n);
	static int32 geometryGetCount(Geometry *g);
	static UserDataArray *geometryGet(Geometry *g, int32 n);
	static int32 geometryFindIndex(Geometry *g, const char *name);

	static int32 frameAdd(Frame *f, const char *name, int32 datatype, int32 numElements);
	static void frameRemove(Frame *f, int32 n);
	static int32 frameGetCount(Frame *f);
	static UserDataArray *frameGet(Frame *f, int32 n);
	static int32 frameFindIndex(Frame *f, const char *name);

	static int32 cameraAdd(Camera *c, const char *name, int32 datatype, int32 numElements);
	static void cameraRemove(Camera *c, int32 n);
	static int32 cameraGetCount(Camera *c);
	static UserDataArray *cameraGet(Camera *c, int32 n);
	static int32 cameraFindIndex(Camera *c, const char *name);

	static int32 lightAdd(Light *l, const char *name, int32 datatype, int32 numElements);
	static void lightRemove(Light *l, int32 n);
	static int32 lightGetCount(Light *l);
	static UserDataArray *lightGet(Light *l, int32 n);
	static int32 lightFindIndex(Light *l, const char *name);

	static int32 materialAdd(Material *m, const char *name, int32 datatype, int32 numElements);
	static void materialRemove(Material *m, int32 n);
	static int32 materialGetCount(Material *m);
	static UserDataArray *materialGet(Material *m, int32 n);
	static int32 materialFindIndex(Material *m, const char *name);

	static int32 textureAdd(Texture *t, const char *name, int32 datatype, int32 numElements);
	static void textureRemove(Texture *t, int32 n);
	static int32 textureGetCount(Texture *t);
	static UserDataArray *textureGet(Texture *t, int32 n);
	static int32 textureFindIndex(Texture *t, const char *name);
};

struct UserDataExtension
{
	int32 numArrays;
	UserDataArray *arrays;

	int32 add(const char *name, int32 datatype, int32 numElements);
	void remove(int32 n);
	int32 getCount(void) { return numArrays; }
	UserDataArray *get(int32 n) { return n >= numArrays ? nil : &arrays[n]; }
	int32 findIndex(const char *name);

	static UserDataExtension *get(Geometry *geo);
	static UserDataExtension *get(Frame *frame);
	static UserDataExtension *get(Camera *cam);
	static UserDataExtension *get(Light *light);
	static UserDataExtension *get(Material *mat);
	static UserDataExtension *get(Texture *tex);
};

struct UserDataGlobals
{
	int32 geometryOffset;
	int32 frameOffset;
	int32 cameraOffset;
	int32 lightOffset;
	int32 materialOffset;
	int32 textureOffset;
};
extern UserDataGlobals userDataGlobals;

void registerUserDataPlugin(void);

}
