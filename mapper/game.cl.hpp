////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/shingle.hpp"
#include "../roengl/muldraw.hpp"
#include "../shing/boxtree.hpp"
#include "../consi/canosi.hpp"
#include "../geo/shp.cl.hpp"
#include "../geo/meshblocks.cl.hpp"
#include "../geo/processed_shape.cl.hpp"
#include "../shing/coordrange.hpp"
#include "../shing/eostri.hpp"
#include "../shing/moremath.hpp"
#include "../roengl/roengl_classdef.hpp"
#include "../shing/schmap.hpp"
#include "../shing/classdef.hpp"
#include "../roengl/touch.hpp"
#include "../roengl/roengl.hpp"
#include "../shing/rva.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class TestClothPoint{
   public:
   //-------------------------------------------------------
   Quaternion quat;
   Point2D clothlean;
   float anglediff = 0;
   float distancepoint = 0;
   Coord3D pos;
   float padding = 0;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Suburb{
   public:
   //-------------------------------------------------------
   FloatRect2D ztorect;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Meshy{
   public:
   //-------------------------------------------------------
   Game* game = NULL;
   uint64_t mbnum = 0;
   MeshBlock* mb = NULL;
   PostCode* postcode = NULL;
   SA1Data* sa1data = NULL;
   FloatRect2D rect;
   RVARange mainrange;
   RVARange flatrange;
   ColorInfo main_color;
   float main_height = 0;
   ChasingFloat popness;
   bool hover_active = false;
   float last_hover_dist = 0;
   //-------------------------------------------------------
   Meshy();
   void update();
   void recolor();
   void reheight();
   void deactivate();
   void activate();
   void drawHoverSmulPart(RVA& bigrva);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Game{
   public:
   //-------------------------------------------------------
   Consi::Consi* detailscontainer = NULL;
   Meshy* last_detailed = NULL;
   int detail_focus_counter = 0;
   Xnl has_focus_xnl;
   RVA mainrva;
   bool mainrva_needresynmc = false;
   RVA flatrva;
   MBHost mbhost;
   FloatRect2D total_rect;
   schmap<uint64_t,Meshy> meshies;
   Shing::Boxtree<Meshy*> meshytree;
   set<Meshy*> activehovers;
   Meshy* closesthover = NULL;
   Point2D last_flatmouse;
   ProcessedShapeFile psf;
   int current_age = 0;
   //-------------------------------------------------------
   Point2D localiseNSWPos(const Point2D& lepos);
   Game();
   void initColorizers();
   void showAge(int theage);
   void initCanosi();
   void maybeResyncMeshVBO();
   void greenColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc);
   void redColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc);
   void blueColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc);
   void heightMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc);
   void colorizeMode(const ColorInfo& lecol, function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc);
   void applyMode(eostri mode, function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc);
   void initMeshies();
   bool handleTouchEvent(const TouchInfo& tinfoil);
   void setup();
   void print();
   void draw2D();
   void draw3D();
   void update();
   void syncDetailFocus();
   void drawPostParticles();
   void drawPostWave();
   void handleMarsTap(const TouchInfo& tinfoil);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
DECLcanosi(details_canosi);
DECLshingle(game, Game);
////////////////////////////////////////////////////////////
