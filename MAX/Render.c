#define FPS 10
#define INLINE inline

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "agl/agl.h"
#include <GL/glut.h>

#include "afs/afs.h"
#include "afs/afsmangle.h"
#include "load.c"

int play_frame = 2;
int play_back_frame = 0;
float frame;
node_st *max3d_camera;
int window_w,window_h;

node_st *node_by_name(node_st* node,char* name){
  while(node){
    Class_Node *n=node->data;
    if(strcmp(n->name,name)==0) return node;
    node=node->next;
  }
  return NULL;
}

void draw_scene(){
  node_st *node;
  Matrix cammat;
  Matrix trmat;
  Class_Node *camnode=max3d_camera->data;

  update(frame);  // update Tracks and Nodes
  
  mat_inverse_cam(cammat,camnode->mat);

  aglInit();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(45.0,(float)window_w / (float)window_h,1.0,10000.0);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // render objects
  printf("Rendering objs from %s   frame=%f\n",camnode->name,frame);
//  mat_print(cammat); mat_test(cammat);
  
  node=scene.Nodes; while(node){
    Class_Node *n=node->data;
    Point3 pos;
    Point3 pos0;
    pos0.x=pos0.y=pos0.z=0;
//    mat_mul(trmat,n->mat,cammat);
    mat_mul(trmat,cammat,n->mat);
    mat_mulvec(&pos,&pos0,trmat);
//    printf("Node '%s':  %8.3f  %8.3f  %8.3f\n",n->name,pos.x,pos.y,pos.z);
    glColor3ubv(n->wirecolor);
    if(n->mesh){
      Class_EditableMesh *mesh=n->mesh;
      int i;
        glBegin(GL_LINES);
        for(i=0;i<mesh->numfaces;i++){
          Face* f=&mesh->faces[i];
          Vertex *v1=&mesh->vertices[f->verts[0]];
          Vertex *v2=&mesh->vertices[f->verts[1]];
          Vertex *v3=&mesh->vertices[f->verts[2]];
          Point3 p1,p2,p3;
          mat_mulvec(&p1,&v1->p,trmat);
          mat_mulvec(&p2,&v2->p,trmat);
          mat_mulvec(&p3,&v3->p,trmat);
          if(f->flags&EDGE_A){ glVertex3f(p1.x,p1.y,p1.z); glVertex3f(p2.x,p2.y,p2.z);}
          if(f->flags&EDGE_B){ glVertex3f(p2.x,p2.y,p2.z); glVertex3f(p3.x,p3.y,p3.z);}
          if(f->flags&EDGE_C){ glVertex3f(p3.x,p3.y,p3.z); glVertex3f(p1.x,p1.y,p1.z);}
        }
        glEnd();
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

  glutSwapBuffers();
  
}

//========================== OpenGL RuleZ ==================================

static void key(unsigned char k, int x, int y){
  switch (k) {
    case 27: exit(0);  /* Escape */
    case 'p':  printf("frame=%f\n",frame);
  }
}

static void mouse(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:    ++play_frame; break;
//    case GLUT_MIDDLE_BUTTON:  resize_window(640,480); break;
    case GLUT_RIGHT_BUTTON:   ++play_back_frame; break;
  }
}

static void idle(void){
	if(play_frame&2){ frame+=FPS; glutPostRedisplay(); } else
	if(play_back_frame&2){   frame-=FPS;  glutPostRedisplay();	}
}

GLvoid resize_window(int w, int h){ 
  glViewport(0,0,w,h); window_w=w;window_h=h;
}

int main(int argc,char* argv[]){
  OLE2_File *of;
  afs_FILE *f1;
  afs_FILE *f2;

    // Load Scene:
    afs_init("",AFS_TYPE_FILES);
    of=OLE2_Open(fopen((argc>1)?argv[1]:"knotmesh-anim.max","rb"));
    if(!of){ printf("File not found!\n");exit(1);}
    f1=afs_open_OLE2(of,"ClassDirectory3");
    if(!f1) f1=afs_open_OLE2(of,"ClassDirectory2");
    if(!f1){ printf("Invalid MAX file (missing ClassDirectory2/3)!\n");exit(1);}
    f2=afs_open_OLE2(of,"Scene");
    if(!f2){ printf("Invalid MAX file (missing Scene)!\n");exit(1);}
    if(load_scene(f1,f2)) {printf("Error reading scene.\n"); return 1;} // error
    afs_fclose(f1); afs_fclose(f2);
    OLE2_Close(of);

    max3d_camera=node_by_name(scene.Nodes,"Camera01");
    if(!max3d_camera) max3d_camera=node_by_name(scene.Nodes,"rolli");
    if(!max3d_camera) {printf("No camera found!!!\n"); return 1;} // error

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (640, 480);
//    glutInitWindowSize (1024, 768);
//    glutInitWindowSize (1280, 1024);
    glutInitWindowPosition (0, 0);
    glutCreateWindow (argv[0]);
//    glutFullScreen();

    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth( 1.0 );
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    resize_window(640,480);
    glutDisplayFunc(draw_scene);
    glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    glutReshapeFunc(resize_window);
    glutIdleFunc(idle);
    glutMainLoop();
  return 0;
}





