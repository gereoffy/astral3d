//========================================================================//
//               Class readers
//========================================================================//
static int extra_refs=0;

#include "trackrdr.c"
#include "textrdr.c"
#include "noderdr.c"
#include "materrdr.c"
#include "pblock.c"
#include "meshrdr.c"
#include "lookatrd.c"
//#include "scenerdr.c"

void init_classreaders(){
  // Track/Key reader:
  register_classreader("Linear Float",CLASSTYPE_TRACK,0x01,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Float",CLASSTYPE_TRACK,0x11,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Float",   CLASSTYPE_TRACK,0x21,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Color",CLASSTYPE_TRACK,0x02,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Color",CLASSTYPE_TRACK,0x12,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Color",   CLASSTYPE_TRACK,0x22,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Position",CLASSTYPE_TRACK,0x03,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Position",CLASSTYPE_TRACK,0x13,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Position",   CLASSTYPE_TRACK,0x23,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Rotation",CLASSTYPE_TRACK,0x04,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Rotation",CLASSTYPE_TRACK,0x14,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Rotation",CLASSTYPE_TRACK,0x24,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Scale",CLASSTYPE_TRACK,0x05,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Scale",CLASSTYPE_TRACK,0x15,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Scale",CLASSTYPE_TRACK,0x25,track_init,track_chunk_reader,track_uninit);
  // Text:
  register_classreader("Text",CLASSTYPE_SHAPE,0,NULL,text_chunk_reader,NULL);
  // Node:
  register_classreader("Node",CLASSTYPE_NODE,0,nodeclass_init,nodeclass_chunk_reader,nodeclass_uninit);
  // Map/Material:
  register_classreader("Standard",CLASSTYPE_MATERIAL,0,NULL,material_chunk_reader,NULL);
  register_classreader("Bitmap",CLASSTYPE_MAP,1,NULL,material_chunk_reader,NULL);
  // ParamBlock:
  register_classreader("ParamBlock",CLASSTYPE_PARAMBLOCK,0,paramblock_init,paramblock_chunk_reader,paramblock_uninit);
  // Editable Mesh / Object
  register_classreader("Editable Mesh",CLASSTYPE_MESH,0,mesh_init,mesh_chunk_reader,mesh_uninit);
  register_classreader("Torus Knot",CLASSTYPE_MESH,1,NULL,NULL,NULL);
  // Orientation
  register_classreader("Position/Rotation/Scale",CLASSTYPE_ORIENTATION,1,NULL,NULL,NULL);
  register_classreader("Look At",CLASSTYPE_ORIENTATION,2,lookat_init,lookat_chunk_reader,lookat_uninit);
  // etc
//  register_classreader("Scene",CLASSTYPE_SCENE,0,NULL,scene_chunk_reader,NULL);
  
}
