
#ifndef CLASS_OCT_EXPORT_STEP
#define CLASS_OCT_EXPORT_STEP

TopoDS_Shape draw_node(cOctNode &node){
  
  gp_Pnt pmin;
  gp_Pnt pmax;

  for (int j=0;j<3; j++){
    pmin.SetCoord(j+1, node.position[j] - 0.5*node.size);
    pmax.SetCoord(j+1, node.position[j] + 0.5*node.size);
    }

  BRepPrimAPI_MakeBox boxMaker(pmin, pmax);
  TopoDS_Shape box = boxMaker.Shape();
  return box;
}

void export_step_model(cOctree &o, string filename){
  BRep_Builder aB;
  TopoDS_Compound aResComp;
  aB.MakeCompound(aResComp);
  
  for (cOctNode *node : o.get_Inside()){
    aB.Add(aResComp, draw_node(*node));
  }
  // Write resulting compound to the file.
  STEPControl_Writer aWriter;
  IFSelect_ReturnStatus aStat = aWriter.Transfer(aResComp,STEPControl_AsIs);
  aStat = aWriter.Write(filename.c_str());
  if(aStat != IFSelect_RetDone) cout << "Writing error" << endl;
}

#endif
