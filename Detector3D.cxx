///////////////////////////////////////
/*  Detector3D.h  7/17/14
    
    Ben Davis-Purcell

    Class that joins all detectors in 3D view
    and handles 3D full and partial tracks

*/
///////////////////////////////////////

#include "Detector3D.h"
#include "TMath.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "TGeoSphere.h"
#include "TWire3D.h"
#include "GetVariables.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGLViewer.h"
#include "TGLPerspectiveCamera.h"

#include <map>
#include <string>
#include <vector>

#define DEG2RAD 3.1415926/180.

using namespace std;

Detector3D::Detector3D()
{

      mgr = new TGeoManager("Geom", "Detector Stack");
      TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
      TGeoMedium *medium =  new TGeoMedium("Vacuum",1, matVacuum);
      mgr->SetVisOption(1);
      mgr->SetVisLevel(6);
      top = mgr->MakeBox("TOP",medium,600,300,300);
      mgr->SetTopVolume(top); 
      mgr->SetVerboseLevel(0);

    string PN[6]={"x", "y", "u", "v", "yp", "xp"};
    vector<string> PlaneNames(&PN[0],&PN[0]+6);
    // First MWDC
     MWDC1 = new WireChamber3D((char*) "MWDC1",  PlaneNames, top, mgr);

     //MWDC1-> LeftWall-> InvisibleAll(kFALSE);
     //MWDC1-> UpperWall->InvisibleAll(kTRUE);

    // Second MWDC
     MWDC2 = new WireChamber3D((char*) "MWDC2",  PlaneNames, top, mgr);


      // s1x - Scintillation Plane     
      s1xplane = new ScintPlane3D((char*)"s1x",top);

      // s1y - Scintillation Plane  
      s1yplane = new ScintPlane3D((char*)"s1y",top);
      
      //s2x Scint Plane
      s2xplane = new ScintPlane3D((char*)"s2x",top);

      //s2y Scint Plane
      // changed volume size to 60
      s2yplane = new ScintPlane3D((char*)"s2y",top);    

      // There will be 10 tracks for any fixed number of tracks in this code,
      // The reason for it is after you call CloseGeometry() function, you
      // cannot dynamically change number of TGeoVolume in root.
      for(int i=0; i<10; i++){
        TrackList.push_back(new Trajectory3D(top,mgr,i));
      }

      mgr->CloseGeometry();
      //top->Raytrace();
      top->Draw("ogl");
      TGLViewer * v = (TGLViewer *)gPad->GetViewer3D();
      if( v != NULL ) {
        static TGLViewer::ECameraType CamType=TGLViewer::kCameraPerspXOY;
        static Double_t fov = 30;
        static Double_t dolly = 300.0;            // FIXME: should be computed somehow
        static Double_t center[3] = {150, 0, 0};  // FIXME: should be fetched from bounding box
        static Double_t vRotate = -40.0*DEG2RAD;  // ROOT docs say degrees, it's Radians...
        static Double_t hRotate = -40.0*DEG2RAD;  // ROOT docs say degrees, it's Radians...
        v->SetResetCamerasOnUpdate(kFALSE);
        v->SetPerspectiveCamera (CamType,fov,dolly,center,hRotate,vRotate);
        v->SetCurrentCamera(CamType);
      }
      cout<<"Detector Created!"<<endl;
}


Detector3D::~Detector3D()
{

}


void Detector3D::ClearTracks()
{
  for (int i=0; i<TrackList.size(); i++) {
    TrackList[i]->Disable();
  }
}
