#include "stdafx.h"
#include "MaxEnv.h"
#include "DataDefine.h"
#include "xcomdoc/xcomdoc.h"
#include "MaxMeshNode.h"
#include "xExportFunction.h"
#include "xXmlDocument.h"

void  CModelBaseExportor::export_mesh_data(xMeshData& meshData , std::wstring& mesh_dir , xcomdoc& doc , sSkeletonID_t&  sid  , bool SkinMesh , bool bTextMode )
{
	int compress_rate =  CMaxEnv::GetInstance().getCompressRate();
	//exportMeshDescToStream(mesh_dir , doc , meshData,0,sid);
	exportMeshDescToStreamAsXML(mesh_dir , doc , meshData,0,sid);

	//写入Mesh的Material
	exportMaterialToStream(mesh_dir , doc , meshData);
	
	//写入face Index. Index buffer
	exportFaceToStream(mesh_dir , doc , meshData);
	if(bTextMode)  exportFaceToStreamAsXML(mesh_dir , doc , meshData);

	if(!SkinMesh)
	{
		//写入Position
		_export_svectors(meshData.m_VertexData.m_Positons , doc , mesh_dir , L"position",compress_rate);
		if(bTextMode)  _export_svectorsAsXML(meshData.m_VertexData.m_Positons , doc , mesh_dir , L"position" , L"MeshVertex" , compress_rate);
	}
	else
	{
		//写入weight
		exportWeightToStream(mesh_dir , doc, meshData , compress_rate );
		if(bTextMode)  exportWeightToStreamAsXML(mesh_dir , doc, meshData , compress_rate );
	}



	//写入Normal
	_export_svectors(meshData.m_VertexData.m_Normals , doc , mesh_dir , L"normal",compress_rate);
	if(bTextMode)  _export_svectorsAsXML(meshData.m_VertexData.m_Normals , doc , mesh_dir , L"normal", L"MeshNormal",  compress_rate);

	//写入平滑的顶点法向量
	if(CMaxEnv::singleton().m_bExpVertexNormal)
	{
		_export_svectors(meshData.m_VertexData.m_VNormals , doc , mesh_dir , L"vnormal",compress_rate);
		if(bTextMode)  _export_svectorsAsXML(meshData.m_VertexData.m_VNormals , doc , mesh_dir , L"vnormal", L"MeshFaceNormal" , compress_rate);
	}

	//写入Diffuse
	exportDiffuseToStream(mesh_dir , doc, meshData , compress_rate );
	if(bTextMode)  exportDiffuseToStreamAsXML(mesh_dir , doc, meshData , compress_rate );
	//写入Tangent
	_export_svectors(meshData.m_VertexData.m_Tangents , doc , mesh_dir , L"tangent",compress_rate);
	if(bTextMode)  _export_svectorsAsXML(meshData.m_VertexData.m_Tangents , doc , mesh_dir , L"tangent", L"MeshTangent", compress_rate);
	//写入UVChannel
	
	exportUVChannelToStream(mesh_dir , doc, meshData , compress_rate );
	if(bTextMode)  exportUVChannelToStreamAsXML(mesh_dir , doc, meshData , compress_rate );
}


bool  CModelBaseExportor::exportMeshDescToStreamAsXML( std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData , int nFrame , sSkeletonID_t sid )
{
	wstring mesh_desc_file = mesh_dir + L"/desc.xml";
	//创建数据流
	xcomdocstream* pDescStream = doc.create_stream(mesh_desc_file.c_str(),(xcd_data_type)cddt_static_mesh_desc);

	sMeshDesc_t* pDesc = meshData.m_pMeshDesc;
	pDesc->m_nVertex = (int)meshData.m_VertexData.m_Positons.size();

	xXmlDocument xmlDoc ;
	xmlDoc.setXMLStyle(true);
	xXmlNode* pRootNode = xmlDoc.insertNode(L"mesh");
	{
		pRootNode->setValue(L"name"          , pDesc->m_Name);
		pRootNode->setValue(L"nSubMesh"      , pDesc->m_nSubMesh);
		pRootNode->setValue(L"nUVMaps"       , pDesc->m_nUVMaps);
		pRootNode->setValue(L"nFace"         , pDesc->m_nFace);
		pRootNode->setValue(L"nVertex"       , pDesc->m_nVertex);
		pRootNode->setValue(L"Fps"           , meshData.m_fFps);
		if(nFrame != 0)
		{
			pRootNode->setValue(L"nFrame"    , nFrame );
		}
		if(CMaxEnv::singleton().m_bInvertTexCoordV )
			pRootNode->setValue(L"TexCoordStyle" , L"Direct3D");
		else
		    pRootNode->setValue(L"TexCoordStyle" , L"OpenGL");
	}

	{
		xXmlNode* bbNode = pRootNode->insertNode(L"BoundBox");
		bbNode->insertValue(L"min_x")->setValue(meshData.m_BoundBox.m_minx);
		bbNode->insertValue(L"min_y")->setValue(meshData.m_BoundBox.m_miny);
		bbNode->insertValue(L"min_z")->setValue(meshData.m_BoundBox.m_minz);
		bbNode->insertValue(L"max_x")->setValue(meshData.m_BoundBox.m_maxx);
		bbNode->insertValue(L"max_y")->setValue(meshData.m_BoundBox.m_maxy);
		bbNode->insertValue(L"max_z")->setValue(meshData.m_BoundBox.m_maxz);

	}
	{
		xXmlNode* pSidNode = pRootNode->insertNode(L"skeleton");
		pSidNode->setValue(L"LoID" , sid.m_HiWord);
		pSidNode->setValue(L"HiID" , sid.m_LoWord);
	}

	{
		xXmlNode* pFurNode = pRootNode->insertNode(L"fur");
		pFurNode->setValue(L"furShellHeight" , pDesc->m_FurInfo.m_furShellHeight);
		pFurNode->setValue(L"furLevel"       , pDesc->m_FurInfo.m_furLevel);
		pFurNode->setValue(L"isFur"          , pDesc->m_FurInfo.m_isFur);
		pFurNode->setValue(L"SkinUTile"      , pDesc->m_FurInfo.m_SkinUTile);
		pFurNode->setValue(L"SkinVTile"      , pDesc->m_FurInfo.m_SkinVTile);
	}
	for(int i = 0 ; i < pDesc->m_nSubMesh ; i ++)
	{
		xXmlNode* pSubNode = pRootNode->insertNode(L"submesh");
		pSubNode->setValue(L"nFace"    , pDesc->m_SubMesh[i].m_nFace);
		pSubNode->setValue(L"nMatIndex", pDesc->m_SubMesh[i].m_nMatIndex);
	}

	xcdstream out_stream(pDescStream);
	xmlDoc.save(out_stream);
	xmlDoc.unload();
	out_stream.close();
	doc.close_stream(pDescStream);

	return true;
}


bool  CModelBaseExportor::exportMeshDescToStream( std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData , int nFrame , sSkeletonID_t sid)
{
	//binaray
	{
		wstring mesh_desc_file = mesh_dir + L"/desc";
		//创建数据流
		xcomdocstream* pDescStream = doc.create_stream(mesh_desc_file.c_str(),(xcd_data_type)cddt_static_mesh_desc);

		//写入Mesh的Description
		pDescStream->write(sid);
		sMeshDesc_t* pDesc = meshData.m_pMeshDesc;
		pDesc->m_nVertex = (int)meshData.m_VertexData.m_Positons.size();
		int desc_size = sizeof(sMeshDesc_t) + (pDesc->m_nSubMesh - 1) * sizeof(sSubMeshDesc_t);
		pDescStream->write(pDesc->m_nSubMesh);
		pDescStream->write((const char*)pDesc,desc_size);
		pDescStream->write(meshData.m_BoundBox);
		doc.close_stream(pDescStream);
	}
	return true;
}

bool  CModelBaseExportor::exportFaceToStreamAsXML( std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData)
{
	wstring mesh_face_file = mesh_dir + L"/face.xml";
	xcomdocstream* pFaceStream = doc.create_stream(mesh_face_file.c_str());
	xXmlDocument xmlDoc ;
	xmlDoc.setXMLStyle(true);

	xXmlNode* pRootNode = xmlDoc.insertNode(L"Mesh");
	pRootNode->setValue(L"nIndexBuffer" , meshData.m_IndexBuffers.size() );
	pRootNode->setValue(L"name"         , mesh_dir.c_str() );
	for(size_t i = 0 ; i < meshData.m_IndexBuffers.size() ; ++i )
	{
		sFace_t* index_buffer  = meshData.m_IndexBuffers[i];
		wchar_t index[32]={0};swprintf(index , L"idx-%d",i);
		xXmlNode* pSubMeshNode = pRootNode->insertNode(index);
		pSubMeshNode->setValue(L"index",i);
		pSubMeshNode->setValue(L"nFace",meshData.m_pMeshDesc->m_SubMesh[i].m_nFace);
		pSubMeshNode->setValue(L"nMatIndex",meshData.m_pMeshDesc->m_SubMesh[i].m_nMatIndex);
		int nFace = meshData.m_pMeshDesc->m_SubMesh[i].m_nFace;
		for(int j = 0 ;  j < nFace ; j ++ )
		{
			wchar_t index[32]={0};swprintf(index , L"idx-%d",j);
			xXmlNode* pFaceNode = pSubMeshNode->insertNode(index);
			sFace_t& _face =  index_buffer[j];
			pFaceNode->setValue(L"A",_face.vert[0]);
			pFaceNode->setValue(L"B",_face.vert[1]);
			pFaceNode->setValue(L"C",_face.vert[2]);
		}
	}

	xcdstream out_stream(pFaceStream);
	xmlDoc.save(out_stream);
	xmlDoc.unload();
	out_stream.close();
	doc.close_stream(pFaceStream);
	return true;
}

bool  CModelBaseExportor::exportFaceToStream( std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData)
{
	wstring mesh_face_file = mesh_dir + L"/face";
	xcomdocstream* pFaceStream = doc.create_stream(mesh_face_file.c_str());
	for(size_t i = 0 ; i < meshData.m_IndexBuffers.size() ; ++i )
	{
		sFace_t* index_buffer  = meshData.m_IndexBuffers[i];
		pFaceStream->write(meshData.m_pMeshDesc->m_SubMesh[i]);
		int nFace = meshData.m_pMeshDesc->m_SubMesh[i].m_nFace;
		pFaceStream->write(index_buffer,nFace);
	}
	doc.close_stream(pFaceStream);
	return true;
}

bool  CModelBaseExportor::exportMaterialToStream(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData)
{
	wstring mesh_matxml_file = mesh_dir + L"/material.xml";
	xcomdocstream* pMatXMLStream = doc.create_stream(mesh_matxml_file.c_str());

	xXmlDocument xml;
	xXmlNode* rootNode = xml.insertNode(L"materials");
	wchar_t* pMaterialName =  (wchar_t*)meshData.m_MatName.c_str();
	wchar_t matScriptName[64]= L"simple";
	int id;
	swscanf(pMaterialName,L"%d - %s",&id,matScriptName);
	if(wcslen(matScriptName) == 0)
		wcsncpy(matScriptName,L"simple",64);
	//if(pMaterialName[0] = '0'&& ( pMaterialName[1] >='0' && pMaterialName[1]<='9') )
	//	pMaterialName = "simple";

	rootNode->setValue(L"matscript", matScriptName );
	for(size_t i = 0 ; i < meshData.m_Materials.size() ; ++i)
	{
		sMaxMatrial_t& mat = meshData.m_Materials[i];
		xXmlNode* matNode = rootNode->insertNode(L"material");
		matNode->setValue(L"index" , (int)i);
		matNode->setValue(L"nTexture" , (int) mat.m_TexLayers.size() );
		matNode->setValue(L"matscript", L"simple");

		matNode->setValue(L"Shiness" , mat.m_BaseMat.m_fShiness );
		matNode->setValue(L"Opactiy" , mat.m_BaseMat.m_fOpactiy );

		xXmlNode* pAmbNode = matNode->insertNode(L"Ambient");
		pAmbNode->setValue(L"r" , mat.m_BaseMat.m_Ambient.r/256.0f ); 
		pAmbNode->setValue(L"g" , mat.m_BaseMat.m_Ambient.g/256.0f ); 
		pAmbNode->setValue(L"b" , mat.m_BaseMat.m_Ambient.b/256.0f ); 
		pAmbNode->setValue(L"a" , mat.m_BaseMat.m_Ambient.a/256.0f ); 
		xXmlNode* pDiffNode = matNode->insertNode(L"Diffuse");
		pDiffNode->setValue(L"r" , mat.m_BaseMat.m_Diffuse.r/256.0f ); 
		pDiffNode->setValue(L"g" , mat.m_BaseMat.m_Diffuse.g/256.0f ); 
		pDiffNode->setValue(L"b" , mat.m_BaseMat.m_Diffuse.b/256.0f ); 
		pDiffNode->setValue(L"a" , mat.m_BaseMat.m_Diffuse.a/256.0f ); 


		xXmlNode* pSpecNode = matNode->insertNode(L"Speculer");
		pSpecNode->setValue(L"r" , mat.m_BaseMat.m_Speculer.r/256.0f ); 
		pSpecNode->setValue(L"g" , mat.m_BaseMat.m_Speculer.g/256.0f ); 
		pSpecNode->setValue(L"b" , mat.m_BaseMat.m_Speculer.b/256.0f ); 
		pSpecNode->setValue(L"a" , mat.m_BaseMat.m_Speculer.a/256.0f ); 

		xXmlNode* pEmissNode = matNode->insertNode(L"Emissive");
		pEmissNode->setValue(L"r" , mat.m_BaseMat.m_Emissive.r/256.0f ); 
		pEmissNode->setValue(L"g" , mat.m_BaseMat.m_Emissive.g/256.0f ); 
		pEmissNode->setValue(L"b" , mat.m_BaseMat.m_Emissive.b/256.0f ); 
		pEmissNode->setValue(L"a" , mat.m_BaseMat.m_Emissive.a/256.0f );

		for(size_t j = 0 ; j < mat.m_TexLayers.size(); ++j)
		{
			xXmlNode* texNode = matNode->insertNode(L"texture");
			texNode->setValue(L"image"  , mat.m_TexLayers[j].m_MTL.m_TexName);
			texNode->setValue(L"channel", mat.m_TexLayers[j].m_MTL.m_UVChannel);
		}
	}

	pMatXMLStream->stream_seekw(xcdsd_beg,0);
	xcdstream out_stream(pMatXMLStream);
	xml.setXMLStyle(true);
	xml.save(out_stream);
	out_stream.close();
	doc.close_stream(pMatXMLStream);

	return true;
}

bool  CModelBaseExportor::exportWeightToStream(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData , int compress_rate)
{
	int nVert = (int)meshData.m_VertexData.m_Positons.size();
	//写入Postion
	wstring mesh_pos_file = mesh_dir + L"/weight";
	xcomdocstream* pPosStream = doc.create_stream(mesh_pos_file.c_str(),xcddt_common, compress_rate);
	pPosStream->write(nVert);
	for(int i = 0 ; i <  nVert ; ++i )
	{
		sWeightedVertex_t & v = meshData.m_VertexData.m_VertexWeights[i];
		pPosStream->write(v);
	}
	doc.close_stream(pPosStream);
	return true;
}

bool  CModelBaseExportor::exportWeightToStreamAsXML(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData , int compress_rate)
{
    int nVert = (int)meshData.m_VertexData.m_Positons.size();
	xXmlDocument xml;
	xXmlNode* rootNode = xml.insertNode(L"MeshWeight");
	rootNode->setValue(L"nVertex",nVert);
	for(int i = 0 ; i <  nVert ; ++i )
	{
		sWeightedVertex_t & v = meshData.m_VertexData.m_VertexWeights[i];
		wchar_t index[32]={0};swprintf(index , L"idx-%d",i);
		xXmlNode* pVertexNode = rootNode->insertNode(index);
		pVertexNode->setValue(L"x" , v.m_InitPos.x);
		pVertexNode->setValue(L"y" , v.m_InitPos.x);
		pVertexNode->setValue(L"z" , v.m_InitPos.x);
		pVertexNode->setValue(L"nEffBone" , v.m_nEffBone);
		for(int j = 0 ; j < v.m_nEffBone ; j ++)
		{
			sVertWeight_t w = v.m_BoneWeights[j];
			wchar_t index[32]={0};swprintf(index , L"idx-%d",j);
			xXmlNode* pWeiNode = pVertexNode->insertNode(index);
			pWeiNode->setValue(L"boneID" , w.m_BoneID);
			pWeiNode->setValue(L"boneIndex" , w.m_BoneIndex);
			pWeiNode->setValue(L"weight" , w.m_fWeight);	
		}
	}

	
	//写入Weight
	wstring mesh_pos_file = mesh_dir + L"/weight.xml";
	xcomdocstream* pWeightStream = doc.create_stream(mesh_pos_file.c_str(),xcddt_common, compress_rate);
	pWeightStream->stream_seekw(xcdsd_beg,0);
	xcdstream out_stream(pWeightStream);
	xml.setXMLStyle(true);
	xml.save(out_stream);
	out_stream.close();
	doc.close_stream(pWeightStream);
	return true;
}

bool  CModelBaseExportor::_export_svectorsAsXML(sVectors_t& vec , xcomdoc& doc , std::wstring mesh_dir , std::wstring stream_name , std::wstring rootNode_name , int compress_rate)
{
	int nVert = (int)vec.size();
	xXmlDocument xml;
	xXmlNode* rootNode = xml.insertNode(rootNode_name.c_str());
	rootNode->setValue(L"nVertex",nVert);
	for(int i = 0 ; i <  nVert ; ++i )
	{
		sVector_t& v = vec[i];
		wchar_t index[32]={0};swprintf(index , L"idx-%d",i);
		xXmlNode* pVecNode = rootNode->insertNode(index);
		pVecNode->setValue(L"x" , v.x);
		pVecNode->setValue(L"y" , v.y);
		pVecNode->setValue(L"z" , v.z);
	}

	//写入Normal
	wstring mesh_vec_file = mesh_dir + L"/" + stream_name + L".xml";
	xcomdocstream* pVecDataStream = doc.create_stream(mesh_vec_file.c_str(),xcddt_common,compress_rate);
	pVecDataStream->stream_seekw(xcdsd_beg,0);
	xcdstream out_stream(pVecDataStream);
	xml.setXMLStyle(true);
	xml.save(out_stream);
	out_stream.close();
	doc.close_stream(pVecDataStream);
	return true;

}

bool CModelBaseExportor::_export_svectors(sVectors_t& vec , xcomdoc& doc , std::wstring mesh_dir , std::wstring stream_name , int compress_rate)
{
	int nVert = (int)vec.size();
	wstring mesh_data_file = mesh_dir + L"/" + stream_name;
	xcomdocstream* pDataStream = doc.create_stream(mesh_data_file.c_str(),xcddt_common,compress_rate);
	pDataStream->write(nVert);
	for(int i = 0 ; i <  nVert ; ++i )
	{
		sVector_t & tang = vec[i];
		pDataStream->write(tang);
	}
	doc.close_stream(pDataStream);
	return true;
}


bool  CModelBaseExportor::exportDiffuseToStream(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData, int compress_rate)
{
	int nVert = (int)meshData.m_VertexData.m_Positons.size();
	wstring mesh_diffuse_file = mesh_dir + L"/diffuse";
	xcomdocstream* pdiffuseStream = doc.create_stream(mesh_diffuse_file.c_str(),xcddt_common,compress_rate);
	pdiffuseStream->write(nVert);
	for(int i = 0 ; i <  nVert ; ++i )
	{
		sColor_t & cl = meshData.m_VertexData.m_Diffuses[i];
		pdiffuseStream->write(cl);
	}
	doc.close_stream(pdiffuseStream);
	return true;
}

bool  CModelBaseExportor::exportDiffuseToStreamAsXML(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData, int compress_rate)
{
	int nVert = (int)meshData.m_VertexData.m_Positons.size();
    xXmlDocument xml;
	xXmlNode* rootNode = xml.insertNode(L"MeshColor");
	rootNode->setValue(L"nVertex",nVert);
	for(int i = 0 ; i <  nVert ; ++i )
	{
		sColor_t & cl = meshData.m_VertexData.m_Diffuses[i];
		wchar_t index[32]={0};swprintf(index , L"idx-%d",i);
		xXmlNode* pVecNode = rootNode->insertNode( index );
		pVecNode->setValue(L"r" , cl.r);
		pVecNode->setValue(L"g" , cl.g);
		pVecNode->setValue(L"b" , cl.b);
		pVecNode->setValue(L"a" , cl.a);
	}

	//写入Diffuse
	wstring mesh_diffuse_file = mesh_dir + L"/diffuse.xml";
	xcomdocstream* pdiffuseStream = doc.create_stream(mesh_diffuse_file.c_str(),xcddt_common,compress_rate);
	pdiffuseStream->stream_seekw(xcdsd_beg,0);
	xcdstream out_stream(pdiffuseStream);
	xml.setXMLStyle(true);
	xml.save(out_stream);
	out_stream.close();
	doc.close_stream(pdiffuseStream);
	return true;

}

bool  CModelBaseExportor::exportUVChannelToStream(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData, int compress_rate)
{
	int nVert = (int)meshData.m_VertexData.m_Positons.size();
	for(int iUVCh = 0 ; iUVCh < meshData.m_VertexData.m_nUVChannes ; ++iUVCh )
	{
		wchar_t buf[32];
		swprintf(buf,L"/uv_%d",iUVCh);
		wstring  file_name = mesh_dir + buf;

		xcomdocstream* pUVStream = doc.create_stream(file_name.c_str(),xcddt_common,compress_rate);
		pUVStream->write(nVert);
		for(int i = 0 ; i <  nVert ; ++i )
		{
			sUVCoord_t & uv = meshData.m_VertexData.m_UVChannels[iUVCh][i];
			pUVStream->write(uv);
		}
		doc.close_stream(pUVStream);
	}
	return true;
}

bool  CModelBaseExportor::exportUVChannelToStreamAsXML(std::wstring mesh_dir , xcomdoc& doc , xMeshData& meshData, int compress_rate)
{
	int nVert = (int)meshData.m_VertexData.m_Positons.size();
	for(int iUVCh = 0 ; iUVCh < meshData.m_VertexData.m_nUVChannes ; ++iUVCh )
	{
		xXmlDocument xml;
		xXmlNode* rootNode = xml.insertNode(L"MeshUV");
		rootNode->setValue(L"nVertex",nVert);
		rootNode->setValue(L"nMapChanel",iUVCh);
		for(int i = 0 ; i <  nVert ; ++i )
		{
			sUVCoord_t & uv = meshData.m_VertexData.m_UVChannels[iUVCh][i];
			wchar_t index[32]={0};swprintf(index , L"idx-%d",i);
			xXmlNode* pUVNode = rootNode->insertNode( index );
			pUVNode->setValue(L"u" , uv.u);
			pUVNode->setValue(L"v" , uv.v);
		}

		wchar_t buf[32];
		swprintf(buf,L"/uv_%d.xml",iUVCh);
		wstring  file_name = mesh_dir + buf;

		xcomdocstream* pUVStream = doc.create_stream(file_name.c_str(),xcddt_common,compress_rate);
		pUVStream->stream_seekw(xcdsd_beg,0);
		xcdstream out_stream(pUVStream);
		xml.setXMLStyle(true);
		xml.save(out_stream);
		out_stream.close();
		doc.close_stream(pUVStream);
	}
	return true;
}

bool CModelBaseExportor::export_mesh_action(std::wstring prefix, xcomdoc& doc , vector<sBoundBox_t>& FrameBoundBoxs , wstring mesh_name,CMaxMesh& mesh,vMaxTimes& exportTimes)
{
	xMeshData&   meshData = mesh.GetOutputData();
	unsigned int iMaxTime = exportTimes[0];
	if(mesh.create(iMaxTime) == false)
		return false;
	if(mesh.build_data_info() == false)
		return false;

	std::wstring mesh_dir  = prefix + mesh_name;
	int nVert = (int)meshData.m_VertexData.m_Positons.size();
	//写入Postion
	std::wstring mesh_pos_file = mesh_dir + L"/position";
	xcomdocstream* pPosStream = doc.create_stream(mesh_pos_file.c_str());
	pPosStream->write(nVert);
	//写入Normal
	std::wstring mesh_norma_file = mesh_dir + L"/normal";
	xcomdocstream* pNormalStream = doc.create_stream(mesh_norma_file.c_str());
	pNormalStream->write(nVert);

	//写入Diffuse
	std::wstring mesh_diffuse_file = mesh_dir + L"/diffuse";
	xcomdocstream* pdiffuseStream = doc.create_stream(mesh_diffuse_file.c_str());
	pdiffuseStream->write(nVert);
	//写入Tangent
	std::wstring mesh_tangent_file = mesh_dir + L"/tangent";
	xcomdocstream* pTangentStream = doc.create_stream(mesh_tangent_file.c_str());
	pTangentStream->write(nVert);

	xcomdocstream* pVNormalStream  = NULL;

	//顶点法向量
	if(CMaxEnv::singleton().m_bExpVertexNormal)
	{
		std::wstring vmesh_norma_file = mesh_dir + L"/vnormal";
		pVNormalStream = doc.create_stream(vmesh_norma_file.c_str());
		pVNormalStream->write(nVert);
	}

	//写入UVChannel
	vector<xcomdocstream*> UVChannelStreams;
	for(int iUVCh = 0 ; iUVCh < meshData.m_VertexData.m_nUVChannes ; ++iUVCh )
	{
		wchar_t buf[32];
		swprintf(buf,L"/uv_%d",iUVCh);
		std::wstring  file_name = mesh_dir + buf;
		xcomdocstream* pUVStream = doc.create_stream(file_name.c_str());
		pUVStream->write(nVert);
		UVChannelStreams.push_back(pUVStream);
	}


	size_t nTimes = exportTimes.size();
	FrameBoundBoxs.clear();
	for(size_t i  = 0 ; i < nTimes ; i++ )// iframe = frame_s ; iframe != frame_e ; iframe += frame_delta)
	{
		sBoundBox_t _bb;

		_bb.clear(100000000000000.0f);
		iMaxTime = exportTimes[i];
		if(i != 0)
		{
			mesh.create(iMaxTime);
			mesh.build_data_info();
		}
		//顶点位置
		for(int i = 0 ; i <  nVert ; ++i )
		{
			sVector_t& v = meshData.m_VertexData.m_Positons[i];
			_bb.add_vertex(v);
			pPosStream->write(v);
		}
		FrameBoundBoxs.push_back(_bb);
		//法向量
		for(int i = 0 ; i <  nVert ; ++i )
		{
			sVector_t& norm = meshData.m_VertexData.m_Normals[i];
			pNormalStream->write(norm);
		}

		//法向量
		if(CMaxEnv::singleton().m_bExpVertexNormal)
		{
			for(int i = 0 ; i <  nVert ; ++i )
			{
				sVector_t& norm = meshData.m_VertexData.m_VNormals[i];
				pVNormalStream->write(norm);
			}
		}

		//颜色
		for(int i = 0 ; i <  nVert ; ++i )
		{
			sColor_t & cl = meshData.m_VertexData.m_Diffuses[i];
			pdiffuseStream->write(cl);
		}

		//Tangent space
		for(int i = 0 ; i <  nVert ; ++i )
		{
			sVector_t & tang = meshData.m_VertexData.m_Tangents[i];
			pTangentStream->write(tang);
		}

		//UV Channel
		for(int iUVCh = 0 ; iUVCh < meshData.m_VertexData.m_nUVChannes ; ++iUVCh )
		{
			xcomdocstream* pUVStream = UVChannelStreams[iUVCh];
			for(int i = 0 ; i <  nVert ; ++i )
			{
				sUVCoord_t & uv = meshData.m_VertexData.m_UVChannels[iUVCh][i];
				pUVStream->write(uv);
			}
		}
	}

	doc.close_stream(pPosStream);
	doc.close_stream(pNormalStream);
	doc.close_stream(pdiffuseStream);
	doc.close_stream(pTangentStream);

	//顶点法向量
	if(CMaxEnv::singleton().m_bExpVertexNormal)
	{
		doc.close_stream(pVNormalStream);
	}
	//doc.close_stream(pFNormalStream);
	for(int iUVCh = 0 ; iUVCh < meshData.m_VertexData.m_nUVChannes ; ++iUVCh )
	{
		xcomdocstream* pUVStream = UVChannelStreams[iUVCh];
		doc.close_stream(pUVStream);
	}
	UVChannelStreams.clear();
	return true;
}

