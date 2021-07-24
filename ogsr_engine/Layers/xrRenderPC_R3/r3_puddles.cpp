#include "stdafx.h"
#include "r2_puddles.h"
#include "..\xrRender\xrRender_console.h"
#include "..\xrRenderDX10\dx10BufferUtils.h"

CPuddles *Puddles;

void SPuddle::make_xform()
{
	Fvector scale = {0,0,0};
	float scale_coeff = radius*0.02f;
	xform.scale(scale_coeff, scale_coeff, scale_coeff);		// константа - обратный радиус модели (14:05:2014 радиус равен 50)
	xform.translate_over(P);
}

void CPuddles::Load()
{
	if (!FS.exist("$level$", "level.puddles"))
		return;

	string_path ini_file;
	FS.update_path(ini_file, "$level$", "level.puddles");
	CInifile ini(ini_file);
	CInifile::Root& sects = ini.sections();
	if (!sects.empty())
	{
		size = (u32)sects.size();
		R_ASSERT2(size <= MAX_PUDDLES, "there are too mush puddles for level (max 30)");
		
		size_t it = 0;
		for (const auto &pairIt : sects)
		{
			const shared_str sect = pairIt.first;
			SPuddle* point = &(points[it]);
			point->P.set(ini.r_fvector3(sect, "center"));
			point->max_depth = ini.r_float(sect, "max_depth");
			point->radius = ini.r_float(sect, "radius");
			point->make_xform();
			it++;
		}
		
		m_bLoaded = true;
	}
	// можно было экспортировать из рендера CModelPool->Instance_Load, да ну нафиг возиться
	string_path		fn;
	if (!FS.exist(fn, "$game_meshes$", "puddles.ogf"))
		Debug.fatal(DEBUG_INFO, "Can't find model puddles.ogf");

	IReader*			data = FS.r_open(fn);
	ogf_header			H;
	data->r_chunk_safe(OGF_HEADER, &H, sizeof(H));
	R_ASSERT2(H.type == MT_NORMAL, "ogse_puddles.ogf must have MT_NORMAL type");

	{	// загрузим меш из модели

		D3DVERTEXELEMENT9	dcl[MAX_FVF_DECL_SIZE];
		D3DVERTEXELEMENT9*	vFormat = 0;
		mesh->dwPrimitives = 0;

		R_ASSERT(data->find_chunk(OGF_VERTICES));
		mesh->vBase = 0;
		u32 fvf = data->r_u32();
		CHK_DX(D3DXDeclaratorFromFVF(fvf, dcl));
		vFormat = dcl;
		mesh->vCount = data->r_u32();
		u32 vStride = D3DXGetFVFVertexSize(fvf);

		VERIFY(NULL == mesh->p_rm_Vertices);

		R_CHK(dx10BufferUtils::CreateVertexBuffer(&mesh->p_rm_Vertices, data->pointer(), mesh->vCount*vStride));

		R_ASSERT(data->find_chunk(OGF_INDICES));
		mesh->iBase = 0;
		mesh->iCount = data->r_u32();
		mesh->dwPrimitives = mesh->iCount / 3;

		VERIFY(NULL == mesh->p_rm_Indices);
		R_CHK(dx10BufferUtils::CreateVertexBuffer(&mesh->p_rm_Indices, data->pointer(), mesh->iCount * 2));

		mesh->rm_geom.create(vFormat, mesh->p_rm_Vertices, mesh->p_rm_Indices);
	}

	FS.r_close(data);
}

void CPuddles::Unload()
{
	xr_delete(mesh);
	size = 0;
	m_bLoaded = false;
}