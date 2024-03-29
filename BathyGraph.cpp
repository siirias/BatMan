#include "BathyGraph.h"

BathyGraph::~BathyGraph()
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
	if (smooth_Vertices)
	{
		delete smooth_Vertices;
		smooth_Vertices = NULL;
	}
	if (smooth_PrimIndices)
	{
		delete smooth_PrimIndices;
		smooth_PrimIndices = NULL;
	}
}


BathyGraph::BathyGraph(scene::ISceneNode * parent, scene::ISceneManager * mgr, s32 id) : scene::ISceneNode(parent, mgr, id)
{
	Vertices = NULL;
	PrimIndices = NULL;
	smooth_Vertices = NULL;
	smooth_PrimIndices = NULL;
	wire = true;
	smooth = true;
	separate_land = true;
	selected_cell = 0;

}

bool BathyGraph::ClearGraphicalData()
{
	if (Vertices != NULL)
		delete Vertices;
	Vertices = NULL;
	if (PrimIndices != NULL)
		delete PrimIndices;
	PrimIndices = NULL;
	if (smooth_Vertices != NULL)
		delete smooth_Vertices;
	smooth_Vertices = NULL;
	if (smooth_PrimIndices != NULL)
		delete smooth_PrimIndices;
	smooth_PrimIndices = NULL;
	return true;
}

bool BathyGraph::RefreshFromData()
{
	float cell_size = 1.0f;
	cells_x = bm_max(1, Bathymetry.cell_sizes.getX());
	cells_y = bm_max(1, Bathymetry.cell_sizes.getY());
	size_x = cells_x*cell_size;
	size_y = cells_y*cell_size;
	std::vector<float>::iterator li;
	max_z = 0.0f;
	for (li = Bathymetry.depths.begin(); li != Bathymetry.depths.end(); li++)
	{
		if (fabs(*li)>max_z)
			max_z = fabs(*li);
	}
	//Gludge to remove annoying earth-points:
	//for (int i = 1; i<Bathymetry.depths.size() - 1; i++)
	//{
	//	if (Bathymetry.depths[i] == 0.0f && Bathymetry.depths[i - 1]<0.0f && Bathymetry.depths[i + 1]<0.0f)
	//		Bathymetry.depths[i] = 0.5f*(Bathymetry.depths[i + 1] + Bathymetry.depths[i - 1]);
	//}
//	depth_multip = (0.15*bm_max(cells_x, cells_y)*cell_size) / max_z;
	depth_multip = cell_size*Bathymetry.depth_in_m/(Bathymetry.x_in_m);
	max_depth_for_drawing = Bathymetry.max_depth_for_drawing;
	cells = 0;
	prims_cell = 3;
	cells = cells_x*cells_y;
	int pr_cell = 4 * 3;
	ClearGraphicalData();
	Vertices = new video::S3DVertex[cells * 4];
	PrimIndices = new u32[cells*pr_cell];
	smooth_Vertices = new video::S3DVertex[cells * 4];
	smooth_PrimIndices = new u32[cells * 4];
	Material.Wireframe = false;
	Material.Lighting = true;
	Material.BackfaceCulling = false;
	Material.AmbientColor = SColor(55, 255, 255, 255);
	Material.DiffuseColor = SColor(55, 255, 255, 255);
	Material.EmissiveColor = SColor(0, 0, 0, 0);
	Material.SpecularColor = SColor(255, 55, 55, 55);
	Material.ColorMaterial = ECM_DIFFUSE_AND_AMBIENT;
	Material.Shininess = 60.0f;
//	Material.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
	WireMaterial.Wireframe = true;
	WireMaterial.Lighting = true;
	WireMaterial.BackfaceCulling = false;
	WireMaterial.AmbientColor = SColor(0, 0, 0, 0);
	WireMaterial.DiffuseColor = SColor(0, 0, 0, 0);
	WireMaterial.EmissiveColor = SColor(0, 40, 20, 20);
	WireMaterial.SpecularColor = SColor(0, 0, 0, 0);
	WireMaterial.ColorMaterial = ECM_NONE;
//	wire = true;
	int vert_num = 0;
	int prim_num = 0;
	int cell_num = 0;
	float puf = 1.0f;
	SColor col = video::SColor(255, 100, 100, 255);
	li = Bathymetry.depths.begin();
//	max_z = 200.0f / depth_multip;//GLUDGE REMOVE
	for (int x = 0; x<cells_x; x++)
	{
		float xp = x*(size_x / cells_x) - 0.5f*size_x;
		for (int y = 0; y<cells_y; y++)
		{
			float yp = y*(size_y / cells_y) - 0.5f*size_y;
			float height = *li*depth_multip;
			li++;
			SColor col_temp = color_from_height(height);
			Vertices[vert_num] = video::S3DVertex(xp, yp, -height, 0, 0, -1, col_temp, 0, 1);
			Vertices[vert_num + 1] = video::S3DVertex(xp, yp + puf*(size_x / cells_x), -height, 0, 0, -1, col_temp, 0, 1);
			Vertices[vert_num + 2] = video::S3DVertex(xp + puf*(size_x / cells_x), yp, -height, 0, 0, -1, col_temp, 0, 1);
			Vertices[vert_num + 3] = video::S3DVertex(xp + puf*(size_x / cells_x), yp + puf*(size_x / cells_x), -height, 0, 0, -1, col_temp, 0, 1);
			vert_num += 4;
			cell_num++;
		}
	}
	cell_num = 0;
	//create the walls
	for (int x = 0; x<cells_x; x++)
	{
		float xp = x*(size_x / cells_x) - 0.5f*size_x;
		for (int y = 0; y<cells_y; y++)
		{
			PrimIndices[prim_num++] = 4 * cell_num;
			PrimIndices[prim_num++] = 4 * cell_num + 1;
			PrimIndices[prim_num++] = 4 * cell_num + 3;
			PrimIndices[prim_num++] = 4 * cell_num + 2;

			int other = cell_num + 1;
			if (y >= cells_y - 1)
				other = cell_num;
			PrimIndices[prim_num++] = 4 * cell_num + 1;
			PrimIndices[prim_num++] = 4 * cell_num + 3;
			PrimIndices[prim_num++] = 4 * other + 2;
			PrimIndices[prim_num++] = 4 * other;

			other = cell_num + cells_y;
			if (x >= cells_x - 1)
				other = cell_num;
			PrimIndices[prim_num++] = 4 * cell_num + 3;
			PrimIndices[prim_num++] = 4 * cell_num + 2;
			PrimIndices[prim_num++] = 4 * other;
			PrimIndices[prim_num++] = 4 * other + 1;
			cell_num++;
		}
	}
	//Create the smooth representation
	vert_num = 0;
	prim_num = 0;
	cell_num = 0;
	for (int x = 0; x<cells_x; x++)
	{
		float xp = x*(size_x / cells_x) - 0.5f*size_x + 0.5f*(size_x / cells_x);
		for (int y = 0; y<cells_y; y++)
		{
			float yp = y*(size_y / cells_y) - 0.5f*size_y + 0.5f*(size_y / cells_y);
			float height = Bathymetry.depths[bm_min(int(Bathymetry.cell_sizes.getTotal()) - 1, cell_num)] * depth_multip;
			float height2 = Bathymetry.depths[bm_min(int(Bathymetry.cell_sizes.getTotal()) - 1, cell_num + 1)] * depth_multip;
			float height3 = Bathymetry.depths[bm_min(int(Bathymetry.cell_sizes.getTotal()) - 1, cell_num + cells_y)] * depth_multip;
			float height4 = Bathymetry.depths[bm_min(int(Bathymetry.cell_sizes.getTotal()) - 1, cell_num + 1 + cells_y)] * depth_multip;
			//std::cout << height << "," << int(depths.size())-1 << cell_num <<"\n";
			SColor col_temp = color_from_height(height);
			smooth_Vertices[vert_num] = video::S3DVertex(xp, yp, -height, 0, 0, -1, col_temp, 0, 1);
			col_temp = color_from_height(height2);
			smooth_Vertices[vert_num + 1] = video::S3DVertex(xp, yp + puf*(size_x / cells_x), -height2, 0, 0, -1, col_temp, 0, 1);
			col_temp = color_from_height(height3);
			smooth_Vertices[vert_num + 2] = video::S3DVertex(xp + puf*(size_x / cells_x), yp, -height3, 0, 0, -1, col_temp, 0, 1);
			col_temp = color_from_height(height4);
			smooth_Vertices[vert_num + 3] = video::S3DVertex(xp + puf*(size_x / cells_x), yp + puf*(size_x / cells_x), -height4, 0, 0, -1, col_temp, 0, 1);
			vert_num += 4;
			cell_num++;
		}
	}
	cell_num = 0;
	//create the walls
	for (int x = 0; x<cells_x; x++)
	{
		float xp = x*(size_x / cells_x) - 0.5f*size_x;
		for (int y = 0; y<cells_y; y++)
		{
			smooth_PrimIndices[prim_num++] = 4 * cell_num;
			smooth_PrimIndices[prim_num++] = 4 * cell_num + 1;
			smooth_PrimIndices[prim_num++] = 4 * cell_num + 3;
			smooth_PrimIndices[prim_num++] = 4 * cell_num + 2;
			cell_num++;
		}
	}

	/*
	The Irrlicht Engine needs to know the bounding box of a scene node.
	It will use it for automatic culling and other things. Hence, we
	need to create a bounding box from the 4 vertices we use.
	If you do not want the engine to use the box for automatic culling,
	and/or don't want to create the box, you could also call
	irr::scene::ISceneNode::setAutomaticCulling() with irr::scene::EAC_OFF.
	*/
	Box.reset(Vertices[0].Pos);
	for (s32 i = 1; i<vert_num; ++i)
		Box.addInternalPoint(Vertices[i].Pos);
	setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	return true;
}

SColor BathyGraph::color_from_height(float height)
{
	SColor Col1(255, 255, 255, 255);
	SColor Col2(255, 0, 0, 255);
	float tmp_max_z = max_z;
	if (max_depth_for_drawing > 0)
		tmp_max_z = max_depth_for_drawing;
	float col_m = fabs(height) / (tmp_max_z*depth_multip);// *6.0f;
	if (col_m>1.0f)
		col_m = 1.0f;
	SColor col_temp = SColor(255,
		(int)((float)Col1.getRed()*(1.0f - col_m) + (float)Col2.getRed()*(col_m)),
		(int)((float)Col1.getGreen()*(1.0f - col_m) + (float)Col2.getGreen()*(col_m)),
		(int)((float)Col1.getBlue()*(1.0f - col_m) + (float)Col2.getBlue()*(col_m)));
	if (separate_land && (height >= -0.0001f))
		col_temp = SColor(255, 60, 150, 60);
	return col_temp;
}

vector3df BathyGraph::world_coord_from_index(bm_intvec coord)
{
	vector3df result(0.0f,0.0f,0.0f);
	if (cells_x <= 0.0f || cells_y <= 0.0f)
		return result;
	result.X = bm_min(size_x*0.5f, bm_max(-size_x*0.5f, ((float)coord.X)*(size_x/((float)cells_x))-0.5f*size_x ));
	result.Y = bm_min(size_y*0.5f, bm_max(-size_y*0.5f, ((float)coord.Y)*(size_y / ((float)cells_y)) - 0.5f*size_y));
	//move in to the center of the point:
	result.X += 0.5f*(size_x / cells_x);
	result.Y += 0.5f*(size_y / cells_y);
	return result;
}

bm_intvec BathyGraph::index_from_world_coord(vector3df coord)
{
	bm_intvec result;
	if(size_x <= 0.0f || size_y <= 0.0f)
		return result;
	result.X=(int)bm_min(cells_x-1,bm_max(0,(int)floor(((coord.X+0.5f*size_x) / (size_x))*float(cells_x))));
	result.Y =(int)bm_min(cells_y-1, bm_max(0,(int)floor(((coord.Y+0.5f*size_y) / size_y)*float(cells_y))));
	return result;
}

void BathyGraph::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	ISceneNode::OnRegisterSceneNode();
}

/*
In the render() method most of the interesting stuff happens: The
Scene node renders itself. We override this method and draw the
tetraeder.
*/
void BathyGraph::render()
{
	//		u16 indices[] = {	0,2,3, 2,1,3, 1,0,3, 2,0,1	};
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	ICameraSceneNode *cam = SceneManager->getActiveCamera();
	driver->setMaterial(Material);
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	if (smooth)
		driver->drawVertexPrimitiveList(smooth_Vertices, cells * 4, smooth_PrimIndices, cells, video::EVT_STANDARD, scene::EPT_QUADS, video::EIT_32BIT);
	else
		driver->drawVertexPrimitiveList(Vertices, cells * 4, PrimIndices, cells * 3, video::EVT_STANDARD, scene::EPT_QUADS, video::EIT_32BIT);
	if (wire)
	{
		vector3df orig_pos = getPosition();
		vector3df cam_pos = cam->getPosition();
		vector3df nudge = cam_pos - orig_pos;
		nudge.setLength(20.1f);
		setPosition(orig_pos + nudge);
		driver->setMaterial(WireMaterial);
		if (smooth)
			driver->drawVertexPrimitiveList(smooth_Vertices, cells * 4, smooth_PrimIndices, cells, video::EVT_STANDARD, scene::EPT_QUADS, video::EIT_32BIT);
		else
			driver->drawVertexPrimitiveList(Vertices, cells * 4, PrimIndices, cells * 3, video::EVT_STANDARD, scene::EPT_QUADS, video::EIT_32BIT);
		setPosition(orig_pos);

	}
}


 const core::aabbox3d<f32>& BathyGraph::getBoundingBox() const
{
	return Box;
}

u32 BathyGraph::getMaterialCount() const
{
	return 1;
}

video::SMaterial& BathyGraph::getMaterial(u32 i)
{
	return Material;
}

