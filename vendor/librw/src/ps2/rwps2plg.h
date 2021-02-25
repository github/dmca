namespace rw {
namespace ps2 {

// MatFX plugin

void initMatFX(void);
ObjPipeline *makeMatFXPipeline(void);

// Skin plugin

void initSkin(void);
ObjPipeline *makeSkinPipeline(void);

void insertVertexSkin(Geometry *geo, int32 i, uint32 mask, Vertex *v);
int32 findVertexSkin(Geometry *g, uint32 flags[], uint32 mask, Vertex *v);

Stream *readNativeSkin(Stream *stream, int32, void *object, int32 offset);
Stream *writeNativeSkin(Stream *stream, int32 len, void *object, int32 offset);
int32 getSizeNativeSkin(void *object, int32 offset);

void instanceSkinData(Geometry *g, Mesh *m, Skin *skin, uint32 *data);

void skinPreCB(MatPipeline*, Geometry*);
void skinPostCB(MatPipeline*, Geometry*);

}
}
