#define FPS 1800

#define INLINE inline

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../agl/agl.h"
//#include <GL/glut.h>

#include "../afs/afs.h"
#include "../afs/afsmangle.h"
#include "../timer/timer.h"
#include "../loadmap/loadmaps.h"

#include "max3d.h"

static node_st *nodes=NULL;
static int nodeno=0;

#include "load.c"

//node_st *max3d_camera=NULL;

static node_st *node_by_name(node_st* node,char* name){
  while(node){
    Class_Node *n=node->data;
    if(strcmp(n->name,name)==0) return node;
    node=node->next;
  }
  return NULL;
}

void MAX_draw_scene(Scene *scene,float frame){
  node_st *node;
//  Matrix objmat;
  Matrix cammat;
  Matrix trmat;
  Matrix normat;
  Class_Node *camnode=NULL;

  update(scene,frame);  // update Tracks and Nodes
  
  if(scene->cam){
    camnode=scene->cam->data;
    mat_inverse(cammat,camnode->objmat);
//    mat_inverse(cammat,camnode->mat);
//    mat_inverse_cam(cammat,camnode->mat);
//    mat_copy(cammat,camnode->mat);
  }

  aglInit();
//  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(45.0,640.0f/480.0f,10.0,20000.0);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // render objects
//  printf("Rendering objs from %s   frame=%f\n",camnode?camnode->name:"0,0,0  LookAt 0,0,-1",frame);
//  mat_print(cammat); mat_test(cammat);

  node=scene->Nodes; while(node){
    Class_Node *n=node->data;
    Point3 pos;
    Point3 pos0;
    pos0.x=pos0.y=pos0.z=0;

//    mat_mul(trmat,n->mat,cammat);
//    mat_mul(objmat,n->mat,n->tm_mat);

    if(camnode)
      mat_mul(trmat,cammat,n->objmat);
    else
      mat_copy(trmat,n->objmat);
    mat_normalize(normat,trmat);
    
    mat_mulvec(&pos,&pos0,trmat);
//    printf("Node '%s':  %8.3f  %8.3f  %8.3f\n",n->name,pos.x,pos.y,pos.z);

    glColor3ubv(n->wirecolor);
    if(n->mesh){
      Mesh *mesh=n->mesh;
      float utile=1;
      float vtile=1;
      int normalcolor=1;
      int i;
//        printf("Rendering mesh: %s\n",n->name);
#if 1

        Mesh_Transform(mesh,trmat,normat);

        aglBlend(AGL_BLEND_ADD);
        aglZbuffer(AGL_ZBUFFER_NONE);
        
        if(strncmp(n->name,"Torus Knot",10)==0){
          float theta=0.00152*frame;
          Class_TorusKnot *knot=mesh;
          aglTexture(scene->texture3_id);
          glColor3f(1,0.9,0.6);
//          printf("Knot particles: %d\n",knot->basecurve_segs);
          glBegin(GL_QUADS);
          for(i=0;i<knot->basecurve_segs;i++){
            Point3 *v=&knot->basecurve[i];
            Point3 p;
            float s=10;
            p.x = v->x*trmat[X][X] + v->y*trmat[X][Y] + v->z*trmat[X][Z] + trmat[X][W];
            p.y = v->x*trmat[Y][X] + v->y*trmat[Y][Y] + v->z*trmat[Y][Z] + trmat[Y][W];
            p.z = v->x*trmat[Z][X] + v->y*trmat[Z][Y] + v->z*trmat[Z][Z] + trmat[Z][W];
            p.x += 3*sin(theta*84.672367437);
            p.y += 3*sin(theta*92.937892367);
            p.z += 3*sin(theta*112.456356267);
            glTexCoord2f(0,0); glVertex3f(p.x-s,p.y-s,p.z);
            glTexCoord2f(1,0); glVertex3f(p.x+s,p.y-s,p.z);
            glTexCoord2f(1,1); glVertex3f(p.x+s,p.y+s,p.z);
            glTexCoord2f(0,1); glVertex3f(p.x-s,p.y+s,p.z);
            theta+=0.7625423;
          }
          glEnd();
//          glLineWidth(1.0f);
          if(scene->knot1_alpha){ Vertex *v=mesh->vertices;
            glColor3f(scene->knot1_alpha,scene->knot1_alpha,scene->knot1_alpha);
            aglTexture(0);
            for(i=0;i<*knot->segs;i++){
              int j;
              glBegin(GL_LINE_LOOP);
              for(j=0;j<*knot->sides;j++){
                glVertex3f(v->tp.x,v->tp.y,v->tp.z);
                ++v;
              }
              glEnd();
            }
          }
//          glLineWidth(1.0f);
          if(scene->knot2_alpha){ 
            glColor3f(scene->knot2_alpha,scene->knot2_alpha,scene->knot2_alpha);
            aglTexture(0);
            for(i=0;i<*knot->sides;i++){
              int j;
              glBegin(GL_LINE_LOOP);
              for(j=0;j<*knot->segs;j++){
                Vertex *v=&mesh->vertices[j*(*knot->sides)+i];
                glVertex3f(v->tp.x,v->tp.y,v->tp.z);
                ++v;
              }
              glEnd();
            }
          }
        }
        
//        if(mesh->numtfaces>0) aglTexture(coins_id); else aglTexture(0);
        if(strncmp(n->name,"Sphere",6)==0){
          aglTexture(scene->texture2_id);
          glColor3f(0.3,0.3,0.3);
          normalcolor=0;
          utile=vtile=4;
        } else {
          aglTexture(scene->texture_id);
          utile=0.2;
          vtile=3;
//          glColor3f(1,1,1);
        }
        glBegin(GL_TRIANGLES);
        for(i=0;i<mesh->numfaces;i++){
          Face* f=&mesh->faces[i];
          Vertex *v1=&mesh->vertices[f->verts[0]];
          Vertex *v2=&mesh->vertices[f->verts[1]];
          Vertex *v3=&mesh->vertices[f->verts[2]];
          if(mesh->numtfaces>0){
            TFace* tf=&mesh->tfaces[i];
            TVertex *tv1=&mesh->tvertices[tf->verts[0]];
            TVertex *tv2=&mesh->tvertices[tf->verts[1]];
            TVertex *tv3=&mesh->tvertices[tf->verts[2]];
            // Textured
            glTexCoord2f(utile*tv1->u,vtile*tv1->v);
if(normalcolor) glColor3f(0.5+0.5*v1->tn.x,0.5+0.5*v1->tn.y,0.5-0.5*v1->tn.z);
            glVertex3f(v1->tp.x,v1->tp.y,v1->tp.z);
            glTexCoord2f(utile*tv2->u,vtile*tv2->v);
if(normalcolor) glColor3f(0.5+0.5*v2->tn.x,0.5+0.5*v2->tn.y,0.5-0.5*v2->tn.z);
            glVertex3f(v2->tp.x,v2->tp.y,v2->tp.z);
            glTexCoord2f(utile*tv3->u,vtile*tv3->v);
if(normalcolor) glColor3f(0.5+0.5*v3->tn.x,0.5+0.5*v3->tn.y,0.5-0.5*v3->tn.z);
            glVertex3f(v3->tp.x,v3->tp.y,v3->tp.z);
          } else {
#if 1
            // Envmap
#if 0
            // white
            glTexCoord2f(0.5+0.5*v1->tn.x,0.5+0.5*v1->tn.y);glVertex3f(v1->tp.x,v1->tp.y,v1->tp.z);
            glTexCoord2f(0.5+0.5*v2->tn.x,0.5+0.5*v2->tn.y);glVertex3f(v2->tp.x,v2->tp.y,v2->tp.z);
            glTexCoord2f(0.5+0.5*v3->tn.x,0.5+0.5*v3->tn.y);glVertex3f(v3->tp.x,v3->tp.y,v3->tp.z);
#else
            // Normal-color
            glTexCoord2f(0.5+0.5*v1->tn.x,0.5+0.5*v1->tn.y);
            glColor3f(0.5+0.5*v1->tn.x,0.5+0.5*v1->tn.y,0.5-0.5*v1->tn.z);glVertex3f(v1->tp.x,v1->tp.y,v1->tp.z);
            glTexCoord2f(0.5+0.5*v2->tn.x,0.5+0.5*v2->tn.y);
            glColor3f(0.5+0.5*v2->tn.x,0.5+0.5*v2->tn.y,0.5-0.5*v2->tn.z);glVertex3f(v2->tp.x,v2->tp.y,v2->tp.z);
            glTexCoord2f(0.5+0.5*v3->tn.x,0.5+0.5*v3->tn.y);
            glColor3f(0.5+0.5*v3->tn.x,0.5+0.5*v3->tn.y,0.5-0.5*v3->tn.z);glVertex3f(v3->tp.x,v3->tp.y,v3->tp.z);
#endif
#else
            // Gouraud only
            glColor3f(0.5+0.5*v1->tn.x,0.5+0.5*v1->tn.y,0.5-0.5*v1->tn.z);glVertex3f(v1->tp.x,v1->tp.y,v1->tp.z);
            glColor3f(0.5+0.5*v2->tn.x,0.5+0.5*v2->tn.y,0.5-0.5*v2->tn.z);glVertex3f(v2->tp.x,v2->tp.y,v2->tp.z);
            glColor3f(0.5+0.5*v3->tn.x,0.5+0.5*v3->tn.y,0.5-0.5*v3->tn.z);glVertex3f(v3->tp.x,v3->tp.y,v3->tp.z);
#endif
          }
        }
        glEnd();
#else
        Mesh_Transform(mesh,trmat,NULL);
        glBegin(GL_LINES);
        for(i=0;i<mesh->numfaces;i++){
          Face* f=&mesh->faces[i];
          Vertex *v1=&mesh->vertices[f->verts[0]];
          Vertex *v2=&mesh->vertices[f->verts[1]];
          Vertex *v3=&mesh->vertices[f->verts[2]];
          if(f->flags&EDGE_A){ glVertex3f(v1->tp.x,v1->tp.y,v1->tp.z); glVertex3f(v2->tp.x,v2->tp.y,v2->tp.z);}
          if(f->flags&EDGE_B){ glVertex3f(v2->tp.x,v2->tp.y,v2->tp.z); glVertex3f(v3->tp.x,v3->tp.y,v3->tp.z);}
          if(f->flags&EDGE_C){ glVertex3f(v3->tp.x,v3->tp.y,v3->tp.z); glVertex3f(v1->tp.x,v1->tp.y,v1->tp.z);}
        }
        glEnd();
#endif
    } else {
      // non-mesh object (light/camera/dummy/unimplemented)
      glBegin(GL_TRIANGLES);
        glVertex3f(pos.x-2,pos.y-1,pos.z);
        glVertex3f(pos.x+2,pos.y-1,pos.z);
        glVertex3f(pos.x,pos.y+1,pos.z);
      glEnd();
    }
    node=node->next;
  }

 
}

Scene* LoadMAXScene(char *filename){
  OLE2_File *of;
  afs_FILE *f1;
  afs_FILE *f2;
  afs_FILE *f3;
  Scene *scene=malloc(sizeof(Scene));
//  scene->nodes=NULL; scene->nodeno=0;

    // Load Scene:
    of=OLE2_Open(fopen(filename,"rb"));
    if(!of){ printf("File not found!\n");return 0;}
    f1=afs_open_OLE2(of,"ClassDirectory3");
    if(!f1) f1=afs_open_OLE2(of,"ClassDirectory2");
    if(!f1){ printf("Invalid MAX file (missing ClassDirectory2/3)!\n");return 0;}
    f2=afs_open_OLE2(of,"Scene");
    if(!f2){ printf("Invalid MAX file (missing Scene)!\n");return 0;}
    f3=afs_open_OLE2(of,"Config");
    if(!f3){ printf("Invalid MAX file (missing Config)!\n");return 0;}
    if(load_scene(scene,f3,f1,f2)) {printf("Error reading scene.\n"); return 0;} // error
    afs_fclose(f1); afs_fclose(f2); afs_fclose(f3);
    OLE2_Close(of);

    scene->cam=node_by_name(scene->Nodes,"Camera01");
    if(!scene->cam) scene->cam=node_by_name(scene->Nodes,"rolli");
//    if(!max3d_camera) {printf("No camera found!!!\n"); return 1;} // error

    return scene;
}

