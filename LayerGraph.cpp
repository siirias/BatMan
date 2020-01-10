#include "LayerGraph.h"



LayerGraph::LayerGraph(scene::ISceneNode * parent, scene::ISceneManager * mgr, s32 id) : scene::ISceneNode(parent, mgr, id)
{
	Vertices = NULL;
	PrimIndices = NULL;
	ul_corner = bm_vec(0.0f, 0.0f);
	lr_corner = bm_vec(100.0f, 100.0f);
	set_height = 1.0f;
}


LayerGraph::~LayerGraph()
{
	if (Vertices)
	{
		delete Vertices;
		Vertices = NULL;
	}
	if (PrimIndices)
	{
		delete PrimIndices;
		PrimIndices = NULL;
	}

}
bool LayerGraph::ClearGraphicalData()
{
	if (Vertices != NULL)
		delete Vertices;
	Vertices = NULL;
	if (PrimIndices != NULL)
		delete PrimIndices;
	PrimIndices = NULL;
	return true;
}

bool LayerGraph::RefreshFromData()
{
	float cell_size = 1.0f;
	ClearGraphicalData();
	Vertices = new video::S3DVertex[8];
	PrimIndices = new u32[8];
	Material.Wireframe = true;
	Material.Lighting = true;
	Material.BackfaceCulling = false;
	Material.AmbientColor = SColor(55, 255, 255, 255);
	Material.DiffuseColor = SColor(55, 255, 255, 255);
	Material.EmissiveColor = SColor(0, 0, 0, 0);
	Material.SpecularColor = SColor(255, 55, 55, 55);
	Material.ColorMaterial = ECM_DIFFUSE_AND_AMBIENT;
	Material.Shininess = 60.0f;

	int vert_num = 0;
	int prim_num = 0;
	int cell_num = 0;
	float puf = 1.0f;
	SColor col = video::SColor(255, 100, 100, 255);
	Vertices[0] = video::S3DVertex(ul_corner.X, ul_corner.Y, set_height, 0, 0, -1, SColor(255, 255, 255, 255), 0.0f, 0.0f);
	Vertices[1] = video::S3DVertex(ul_corner.X, lr_corner.Y, set_height, 0, 0, -1, SColor(255, 255, 255, 255), 0.0f, 1.0f);
	Vertices[2] = video::S3DVertex(lr_corner.X, ul_corner.Y, set_height, 0, 0, -1, SColor(255, 255, 255, 255), 1.0f, 0.0f);
	Vertices[3] = video::S3DVertex(lr_corner.X, lr_corner.Y, set_height, 0, 0, -1, SColor(255, 255, 255, 255), 1.0f, 1.0f);
	PrimIndices[0] = 0;
	PrimIndices[1] = 1;
	PrimIndices[2] = 3;
	PrimIndices[3] = 2;
	/*
	The Irrlicht Engine needs to know the bounding box of a scene node.
	It will use it for automatic culling and other things. Hence, we
	need to create a bounding box from the 4 vertices we use.
	If you do not want the engine to use the box for automatic culling,
	and/or don't want to create the box, you could also call
	irr::scene::ISceneNode::setAutomaticCulling() with irr::scene::EAC_OFF.
	*/
	//Box.reset(Vertices[0].Pos);
	//for (s32 i = 1; i<vert_num; ++i)
	//	Box.addInternalPoint(Vertices[i].Pos);
	irr::scene::ISceneNode::setAutomaticCulling(irr::scene::EAC_OFF);
	Box.reset(Vertices[0].Pos);
	for (s32 i = 1; i<vert_num; ++i)
		Box.addInternalPoint(Vertices[i].Pos);

	setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	return true;
}

void LayerGraph::render()
{
	//		u16 indices[] = {	0,2,3, 2,1,3, 1,0,3, 2,0,1	};
//	std::cout << "start render layer\n";

	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	ICameraSceneNode *cam = SceneManager->getActiveCamera();
	driver->setMaterial(Material);
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	//for (int i = 0; i < 4;i++)
	//	std::cout << PrimIndices[i] << "\n";
	//for (int i = 0; i < 4; i++)
	//	std::cout << Vertices[i].Pos.X << " " << Vertices[i].Pos.Y << " " << Vertices[i].Pos.Z << "\n";
	driver->drawVertexPrimitiveList(Vertices, 4, PrimIndices, 3, video::EVT_STANDARD, scene::EPT_QUADS, video::EIT_32BIT);
//	std::cout << "end render layer\n";

}

const core::aabbox3d<f32>& LayerGraph::getBoundingBox() const
{
	return Box;
}

void LayerGraph::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

u32 LayerGraph::getMaterialCount() const
{
	return 1;
}

video::SMaterial& LayerGraph::getMaterial(u32 i)
{
	return Material;
}

