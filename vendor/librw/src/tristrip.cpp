#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "rwbase.h"
#include "rwerror.h"
#include "rwplg.h"
#include "rwpipeline.h"
#include "rwobjects.h"
#include "rwengine.h"

#define PLUGIN_ID 2

namespace rw {

struct GraphEdge
{
	int32 node;	/* index of the connected node */
	uint32 isConnected : 1;	/* is connected to other node */
	uint32 otherEdge : 2;	/* edge number on connected node */
	uint32 isStrip : 1;	/* is strip edge */
};

struct StripNode
{
	uint16 v[3];	/* vertex indices */
	uint8 parent : 2;	/* tunnel parent node (edge index) */
	uint8 visited : 1;	/* visited in breadth first search */
	uint8 stripVisited : 1;	/* strip starting at this node was visited during search */
	uint8 isEnd : 1;	/* is in end list */
	GraphEdge e[3];
	int32 stripId;	/* index of start node */
	LLLink inlist;
};

struct StripMesh
{
	int32 numNodes;
	StripNode *nodes;
	LinkList loneNodes;	/* nodes not connected to any others */
	LinkList endNodes;	/* strip start/end nodes */
};

//#define trace(...) printf(__VA_ARGS__)
#define trace(...)

static void
printNode(StripMesh *sm, StripNode *n)
{
	trace("%3ld: %3d  %3d.%d  %3d.%d %3d.%d || %3d %3d %3d\n",
		n - sm->nodes,
		n->stripId,
		n->e[0].node,
		n->e[0].isStrip,
		n->e[1].node,
		n->e[1].isStrip,
		n->e[2].node,
		n->e[2].isStrip,
		n->v[0],
		n->v[1],
		n->v[2]);
}

static void
printLone(StripMesh *sm)
{
	FORLIST(lnk, sm->loneNodes)
		printNode(sm, LLLinkGetData(lnk, StripNode, inlist));
}

static void
printEnds(StripMesh *sm)
{
	FORLIST(lnk, sm->endNodes)
		printNode(sm, LLLinkGetData(lnk, StripNode, inlist));
}

static void
printSmesh(StripMesh *sm)
{
	for(int32 i = 0; i < sm->numNodes; i++)
		printNode(sm, &sm->nodes[i]);
}

static void
collectFaces(Geometry *geo, StripMesh *sm, uint16 m)
{
	StripNode *n;
	Triangle *t;
	sm->numNodes = 0;
	for(int32 i = 0; i < geo->numTriangles; i++){
		t = &geo->triangles[i];
		if(t->matId == m){
			n = &sm->nodes[sm->numNodes++];
			n->v[0] = t->v[0];
			n->v[1] = t->v[1];
			n->v[2] = t->v[2];
			assert(t->v[0] < geo->numVertices);
			assert(t->v[1] < geo->numVertices);
			assert(t->v[2] < geo->numVertices);
			n->e[0].node = 0;
			n->e[1].node = 0;
			n->e[2].node = 0;
			n->e[0].isConnected = 0;
			n->e[1].isConnected = 0;
			n->e[2].isConnected = 0;
			n->e[0].isStrip = 0;
			n->e[1].isStrip = 0;
			n->e[2].isStrip = 0;
			n->parent = 0;
			n->visited = 0;
			n->stripVisited = 0;
			n->isEnd = 0;
			n->stripId = -1;
			n->inlist.init();
		}
	}
}

/* Find Triangle that has edge e that is not connected yet. */
static GraphEdge
findEdge(StripMesh *sm, int32 e[2])
{
	StripNode *n;
	GraphEdge ge = { 0, 0, 0, 0 };
	for(int32 i = 0; i < sm->numNodes; i++){
		n = &sm->nodes[i];
		for(int32 j = 0; j < 3; j++){
			if(n->e[j].isConnected)
				continue;
			if(e[0] == n->v[j] &&
			   e[1] == n->v[(j+1) % 3]){
				ge.node = i;
				// signal success
				ge.isConnected = 1;
				ge.otherEdge = j;
				return ge;
			}
		}
	}
	return ge;
}

/* Connect nodes sharing an edge, preserving winding */
static void
connectNodesPreserve(StripMesh *sm)
{
	StripNode *n, *nn;
	int32 e[2];
	GraphEdge ge;
	for(int32 i = 0; i < sm->numNodes; i++){
		n = &sm->nodes[i];
		for(int32 j = 0; j < 3; j++){
			if(n->e[j].isConnected)
				continue;

			/* flip edge and search for node */
			e[1] = n->v[j];
			e[0] = n->v[(j+1) % 3];
			ge = findEdge(sm, e);
			if(ge.isConnected){
				/* found node, now connect */
				n->e[j].node = ge.node;
				n->e[j].isConnected = 1;
				n->e[j].otherEdge = ge.otherEdge;
				n->e[j].isStrip = 0;
				nn = &sm->nodes[ge.node];
				nn->e[ge.otherEdge].node = i;
				nn->e[ge.otherEdge].isConnected = 1;
				nn->e[ge.otherEdge].otherEdge = j;
				nn->e[ge.otherEdge].isStrip = 0;
			}
		}
	}
}

static int32
numConnections(StripNode *n)
{
	return  n->e[0].isConnected +
		n->e[1].isConnected +
		n->e[2].isConnected;
}

static int32
numStripEdges(StripNode *n)
{
	return  n->e[0].isStrip +
		n->e[1].isStrip +
		n->e[2].isStrip;
}

#define IsEnd(n) (numConnections(n) > 0 && numStripEdges(n) < 2)

/* Complement the strip-ness of an edge */
static void
complementEdge(StripMesh *sm, GraphEdge *e)
{
	e->isStrip = !e->isStrip;
	e = &sm->nodes[e->node].e[e->otherEdge];
	e->isStrip = !e->isStrip;
}

/* While possible extend a strip from a starting node until
 * we find a node already in a strip. N.B. this function
 * makes no attempts to connect to an already existing strip.
 * It also doesn't try to alternate between left and right. */
static void
extendStrip(StripMesh *sm, StripNode *start)
{
	StripNode *n, *nn;
	n = start;
	if(numConnections(n) == 0){
		sm->loneNodes.append(&n->inlist);
		return;
	}
	sm->endNodes.append(&n->inlist);
	n->isEnd = 1;
loop:
	/* Find the next node to connect to on any of the three edges */
	for(int32 i = 0; i < 3; i++){
		if(!n->e[i].isConnected)
			continue;
		nn = &sm->nodes[n->e[i].node];
		if(nn->stripId >= 0)
			continue;

		/* found one */
		nn->stripId = n->stripId;
		/* We know it's not a strip edge yet,
		 * so complementing it will make it one. */
		complementEdge(sm, &n->e[i]);
		n = nn;
		goto loop;
	}
	if(n != start){
		sm->endNodes.append(&n->inlist);
		n->isEnd = 1;
	}
}

static void
buildStrips(StripMesh *sm)
{
	StripNode *n;
	for(int32 i = 0; i < sm->numNodes; i++){
		n = &sm->nodes[i];
		if(n->stripId >= 0)
			continue;
		n->stripId = i;
		extendStrip(sm, n);
	}
}

static StripNode*
findTunnel(StripMesh *sm, StripNode *n)
{
	LinkList searchNodes;
	StripNode *nn;
	int edgetype;
	int isEnd;

	searchNodes.init();
	edgetype = 0;
	for(;;){
		for(int32 i = 0; i < 3; i++){
			/* Find a node connected by the right edgetype */
			if(!n->e[i].isConnected ||
			    n->e[i].isStrip != edgetype)
				continue;
			nn = &sm->nodes[n->e[i].node];

			/* If the node has been visited already,
			 * there's a shorter path. */
			if(nn->visited)
				continue;

			/* Don't allow non-strip edge between nodes of the same
			 * strip to prevent loops.
			 * Actually these edges are allowed under certain
			 * circumstances, but they require complex checks. */
			if(edgetype == 0 &&
			   n->stripId == nn->stripId)
				continue;

			isEnd = IsEnd(nn);

			/* Can't add end nodes to two lists, so skip. */
			if(isEnd && edgetype == 1)
				continue;

			nn->parent = n->e[i].otherEdge;
			nn->visited = 1;
			sm->nodes[nn->stripId].stripVisited = 1;

			/* Search complete. */
			if(isEnd && edgetype == 0)
				return nn;

			/* Found a valid node. */
			searchNodes.append(&nn->inlist);
		}
		if(searchNodes.isEmpty())
			return nil;
		n = LLLinkGetData(searchNodes.link.next, StripNode, inlist);
		n->inlist.remove();
		edgetype = !edgetype;
	}
}

static void
resetGraph(StripMesh *sm)
{
	StripNode *n;
	for(int32 i = 0; i < sm->numNodes; i++){
		n = &sm->nodes[i];
		n->visited = 0;
		n->stripVisited = 0;
	}
}

static StripNode*
walkStrip(StripMesh *sm, StripNode *start)
{
	StripNode *n, *nn;
	int32 last;

//trace("stripend: ");
//printNode(sm, start);

	n = start;
	last = -1;
	for(;;n = nn){
		n->visited = 0;
		n->stripVisited = 0;
		if(n->isEnd)
			n->inlist.remove();
		n->isEnd = 0;

		if(IsEnd(n) && n != start)
			return n;

		/* find next node */
		nn = nil;
		for(int32 i = 0; i < 3; i++){
			if(!n->e[i].isStrip || i == last)
				continue;
			nn = &sm->nodes[n->e[i].node];
			last = n->e[i].otherEdge;
			nn->stripId = n->stripId;
			break;
		}
//trace("    next: ");
//printNode(sm, nn);
		if(nn == nil)
			return nil;
	}
}

static void
applyTunnel(StripMesh *sm, StripNode *end, StripNode *start)
{
	LinkList tmplist;
	StripNode *n, *nn;

	for(n = end; n != start; n = &sm->nodes[n->e[n->parent].node]){
//trace("	");
//printNode(sm, n);
		complementEdge(sm, &n->e[n->parent]);
	}
//trace("	");
//printNode(sm, start);

//printSmesh(sm);
//trace("-------\n");
	tmplist.init();
	while(!sm->endNodes.isEmpty()){
		n = LLLinkGetData(sm->endNodes.link.next, StripNode, inlist);
		/* take out of end list */
		n->inlist.remove();
		n->isEnd = 0;
		/* no longer an end node */
		if(!IsEnd(n))
			continue;
		// TODO: only walk strip if it was touched
		/* set new id, walk strip and find other end */
		n->stripId = n - sm->nodes;
		nn = walkStrip(sm, n);
		tmplist.append(&n->inlist);
		n->isEnd = 1;
		if(nn && n != nn){
			tmplist.append(&nn->inlist);
			nn->isEnd = 1;
		}
	}
	/* Move new end nodes to the real list. */
	sm->endNodes = tmplist;
	sm->endNodes.link.next->prev = &sm->endNodes.link;
	sm->endNodes.link.prev->next = &sm->endNodes.link;
}

static void
tunnel(StripMesh *sm)
{
	StripNode *n, *nn;

again:
	FORLIST(lnk, sm->endNodes){
		n = LLLinkGetData(lnk, StripNode, inlist);
//		trace("searching %p %d\n", n, numStripEdges(n));
		nn = findTunnel(sm, n);
//		trace("          %p %p\n", n, nn);

		if(nn){
			applyTunnel(sm, nn, n);
			resetGraph(sm);
			/* applyTunnel changes sm->endNodes, so we have to
			 * jump out of the loop. */
			goto again;
		}
		resetGraph(sm);
	}
	trace("tunneling done!\n");
}

/* Get next edge in strip.
 * Last is the edge index whence we came lest we go back. */
static int
getNextEdge(StripNode *n, int32 last)
{
	int32 i;
	for(i = 0; i < 3; i++)
		if(n->e[i].isStrip && i != last)
			return i;
	return -1;
}

#define NEXT(x) (((x)+1) % 3)
#define PREV(x) (((x)+2) % 3)
#define RIGHT(x) NEXT(x)
#define LEFT(x) PREV(x)

/* Generate mesh indices for all strips in a StripMesh */
static void
makeMesh(StripMesh *sm, Mesh *m)
{
	int32 i, j;
	int32 rightturn, lastrightturn;
	int32 seam;
	int32 even;
	StripNode *n;

	/* three indices + two for stitch per triangle must be enough */
	m->indices = rwNewT(uint16, sm->numNodes*5, MEMDUR_FUNCTION | ID_GEOMETRY);
	memset(m->indices, 0xFF, sm->numNodes*5*sizeof(uint16));

	even = 1;
	FORLIST(lnk, sm->endNodes){
		n = LLLinkGetData(lnk, StripNode, inlist);
		/* only interested in start nodes, not the ends */
		if(n->stripId != (n - sm->nodes))
			continue;

		/* i is the edge we enter this triangle from.
		 * j is the edge we exit. */
		j = getNextEdge(n, -1);
		/* starting triangle must have connection */
		if(j < 0)
			continue;
		/* Space to stitch together strips */
		seam = m->numIndices;
		if(seam)
			m->numIndices += 2;
		/* Start ccw for even tris */
		if(even){
			/* Start with a right turn */
			i = LEFT(j);
			m->indices[m->numIndices++] = n->v[i];
			m->indices[m->numIndices++] = n->v[NEXT(i)];
		}else{
			/* Start with a left turn */
			i = RIGHT(j);
			m->indices[m->numIndices++] = n->v[NEXT(i)];
			m->indices[m->numIndices++] = n->v[i];
		}
trace("\nstart %d %d\n", numStripEdges(n), m->numIndices-2);
		lastrightturn = -1;

		while(j >= 0){
			rightturn = RIGHT(i) == j;
			if(rightturn == lastrightturn){
				// insert a swap if we're not alternating
				m->indices[m->numIndices] = m->indices[m->numIndices-2];
trace("SWAP\n");
				m->numIndices++;
				even = !even;
			}
trace("%d:%d%c %d %d %d\n", n-sm->nodes, m->numIndices, even ? ' ' : '.', n->v[0], n->v[1], n->v[2]);
			lastrightturn = rightturn;
			if(rightturn)
				m->indices[m->numIndices++] = n->v[NEXT(j)];
			else
				m->indices[m->numIndices++] = n->v[j];
			even = !even;

			/* go to next triangle */
			i = n->e[j].otherEdge;
			n = &sm->nodes[n->e[j].node];
			j = getNextEdge(n, i);
		}

		/* finish strip */
trace("%d:%d%c %d %d %d\nend\n", n-sm->nodes, m->numIndices, even ? ' ' : '.', n->v[0], n->v[1], n->v[2]);
		m->indices[m->numIndices++] = n->v[LEFT(i)];
		even = !even;
		if(seam){
			m->indices[seam] = m->indices[seam-1];
			m->indices[seam+1] = m->indices[seam+2];
trace("STITCH %d: %d %d\n", seam, m->indices[seam], m->indices[seam+1]);
		}
	}

	/* Add all unconnected and lonely triangles */
	FORLIST(lnk, sm->endNodes){
		n = LLLinkGetData(lnk, StripNode, inlist);
		if(numStripEdges(n) != 0)
			continue;
		if(m->numIndices != 0){
			m->indices[m->numIndices] = m->indices[m->numIndices-1];
			m->numIndices++;
			m->indices[m->numIndices++] = n->v[!even];
		}
		m->indices[m->numIndices++] = n->v[!even];
		m->indices[m->numIndices++] = n->v[even];
		m->indices[m->numIndices++] = n->v[2];
		even = !even;
	}
	FORLIST(lnk, sm->loneNodes){
		n = LLLinkGetData(lnk, StripNode, inlist);
		if(m->numIndices != 0){
			m->indices[m->numIndices] = m->indices[m->numIndices-1];
			m->numIndices++;
			m->indices[m->numIndices++] = n->v[!even];
		}
		m->indices[m->numIndices++] = n->v[!even];
		m->indices[m->numIndices++] = n->v[even];
		m->indices[m->numIndices++] = n->v[2];
		even = !even;
	}
}

static void verifyMesh(Geometry *geo);

/*
 * For each material:
 * 1. build dual graph (collectFaces, connectNodes)
 * 2. make some simple strip (buildStrips)
 * 3. apply tunnel operator (tunnel)
 */
void
Geometry::buildTristrips(void)
{
	int32 i;
	uint16 *indices;
	MeshHeader *header;
	Mesh *ms, *md;
	StripMesh smesh;

//	trace("%ld\n", sizeof(StripNode));

	this->allocateMeshes(matList.numMaterials, 0, 1);

	smesh.nodes = rwNewT(StripNode, this->numTriangles, MEMDUR_FUNCTION | ID_GEOMETRY);
	ms = this->meshHeader->getMeshes();
	for(int32 i = 0; i < this->matList.numMaterials; i++){
		smesh.loneNodes.init();
		smesh.endNodes.init();
		collectFaces(this, &smesh, i);
		connectNodesPreserve(&smesh);
		buildStrips(&smesh);
printSmesh(&smesh);
//trace("-------\n");
//printLone(&smesh);
//trace("-------\n");
//printEnds(&smesh);
//trace("-------\n");
		// TODO: make this work
//		tunnel(&smesh);
//trace("-------\n");
//printEnds(&smesh);

		ms[i].material = this->matList.materials[i];
		makeMesh(&smesh, &ms[i]);
		this->meshHeader->totalIndices += ms[i].numIndices;
	}
	rwFree(smesh.nodes);

	/* Now re-allocate and copy data */
	header = this->meshHeader;
	this->meshHeader = nil;
	this->allocateMeshes(header->numMeshes, header->totalIndices, 0);
	this->meshHeader->flags = MeshHeader::TRISTRIP;
	md = this->meshHeader->getMeshes();
	indices = md->indices;
	for(i = 0; i < header->numMeshes; i++){
		md[i].material = ms[i].material;
		md[i].numIndices = ms[i].numIndices;
		md[i].indices = indices;
		indices += md[i].numIndices;
		memcpy(md[i].indices, ms[i].indices, md[i].numIndices*sizeof(uint16));
		rwFree(ms[i].indices);
	}
	rwFree(header);

	verifyMesh(this);
}

/* Check that tristripped mesh and geometry triangles are actually the same. */
static void
verifyMesh(Geometry *geo)
{
	int32 i, k;
	uint32 j;
	int32 x;
	int32 a, b, c, m;
	Mesh *mesh;
	Triangle *t;
	uint8 *seen;

	seen = rwNewT(uint8, geo->numTriangles, MEMDUR_FUNCTION | ID_GEOMETRY);
	memset(seen, 0, geo->numTriangles);

	mesh = geo->meshHeader->getMeshes();
	for(i = 0; i < geo->meshHeader->numMeshes; i++){
		m = geo->matList.findIndex(mesh->material);
		x = 0;
		for(j = 0; j < mesh->numIndices-2; j++){
			a = mesh->indices[j+x];
			x = !x;
			b = mesh->indices[j+x];
			c = mesh->indices[j+2];
			if(a >= geo->numVertices ||
			   b >= geo->numVertices ||
			   c >= geo->numVertices){
				fprintf(stderr, "triangle %d %d %d out of range (%d)\n", a, b, c, geo->numVertices);
				goto loss;
			}
			if(a == b || a == c || b == c)
				continue;
trace("%d %d %d\n", a, b, c);

			/* now that we have a triangle, try to find it */
			for(k = 0; k < geo->numTriangles; k++){
				t = &geo->triangles[k];
				if(seen[k] || t->matId != m) continue;
				if((t->v[0] == a && t->v[1] == b && t->v[2] == c) ||
				   (t->v[1] == a && t->v[2] == b && t->v[0] == c) ||
				   (t->v[2] == a && t->v[0] == b && t->v[1] == c)){
					seen[k] = 1;
					goto found;
				}
			}
			goto loss;
		found:	;
		}
		mesh++;
	}

	/* Also check that all triangles are in the mesh */
	for(i = 0; i < geo->numTriangles; i++)
		if(!seen[i]){
	loss:
			fprintf(stderr, "TRISTRIP verify failed\n");
			exit(1);
		}

	rwFree(seen);
}

}
