
node_st* link_nodes(int ctype){
  node_st *list=NULL;
  node_st *lend=NULL;
  int i;
  for(i=0;i<nodeno;i++){
    int cid=nodes[i].classid;
    if( (ctype==CLASSTYPE_MODOBJ && cid==0x2032) ||
        (cid>=0 && cid<classdb && classtab[cid].type==ctype) ){
      if(lend){
        lend->next=&nodes[i];
        lend=lend->next;
      } else {
        list=lend=&nodes[i];
      }
    }
  }
  return list;
}

#if 0
void list_nodes(node_st *list){
  while(list){
    Class_Node *n=list->data;
    int prs=list->reflist[2];
    int obj=list->reflist[4];
    printf("Node '%s'  parent: %d\n",n->name,n->parent);
    if(prs>=0 && prs<nodeno){
      node_st *prsnode=&nodes[prs];
      int cid=prsnode->classid;
      if(cid>=0 && cid<classdb && classtab[cid].type==CLASSTYPE_ORIENTATION){
        if(classtab[cid].subtype==1) printf("  PRS\n"); else
        if(classtab[cid].subtype==1) printf("  LookAt\n");
      }
    }
    if(obj>=0 && obj<nodeno){
      node_st *objnode=&nodes[obj];
      int cid=objnode->classid;
      if(cid>=0 && cid<classdb && classtab[cid].type==CLASSTYPE_OBJECT){
        printf("  Object\n");
      } else if(cid==0x2032){
        printf("  Modified Object\n");
      }
    }
    list=list->next;
  }
}
#endif



