////////////////////////////////////////////////////////////
#include "game.cl.hpp"
#include "../shing/moremath.hpp"
#include "../roengl/touch.hpp"
#include "../roengl/utils.hpp"
#include "../mars/threedifier.hpp"
#include "../shing/quickbenchmarker.hpp"
#include "../shing/fixedmath.hpp"
#include "../roengl/muldraw.hpp"
#include "../roengl/uniformbuffer.hpp"
#include "../basegame/basegame.hpp"
#include "../shing/neocontext.hpp"
#include "../roengl/keyceptor.hpp"
#include "../roengl/neocommando.hpp"
#include "../shing/secsy.hpp"
#include "../ncgl/gunlogic.hpp"
#include "../ncgl/anode.cl.hpp"
#include "../geo/shp.cl.hpp"
#include "../include/earcut.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
Point2D Game::localiseNSWPos(const Point2D& lepos){
   return Point2D(lepos.x * 1200, lepos.y * -1000);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Game::Game(){
   mbhost.loadFile(bunschfile("meshblocks.flec").path());
   meshies / [this](auto& lemes, const auto& leindex){
      lemes.mbnum = leindex;
      lemes.game = this;
      lemes.mb = &mbhost.ob(leindex);
      lemes.postcode = &mbhost.postcodes[lemes.mb->postcode];
      lemes.sa1data = &mbhost.sadatas[lemes.mb->sa1_code];
   };
   initMeshies();
   initCanosi();
   BG::marsiso().focusCamOn(Point2D(1091.9, -295.545));
   BG::marsiso().setCamDist(601);
   BG::marsiso().setCamDir(-155);
   initColorizers();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::initColorizers(){
   auto tree_perc_func = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return mb.bigtree_perc / 20.0f;
   };
   auto veg_perc = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return (100 - mb.nonveg_perc) / 100.0f;
   };
   auto urbanheat = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return mb.uhi_index / 8.0f;
   };
   auto pop_num = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return mb.pop_2016 / 200;
   };
   auto age_1 = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return sadata.agedPop(0) / 20.0f;
   };
   auto greyer = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return 0.5f;
   };
   auto randomer = [this](MeshBlock& mb, SA1Data& sadata, SA2Data& satwo) -> float{
      return floatrandbetween(0, 1);
   };
   { int _rangetotal = 6;
   for(int i = 0; i < _rangetotal; i++){
      eostri mode;
      if(i == 0){
         mode = e__(red);
      }
      else if(i == 1){
         mode = e__(green);
      }
      else if(i == 2){
         mode = e__(blue);
      }
      else if(i == 3){
         mode = e__(greenifier);
      }
      else if(i == 4){
         mode = e__(grey);
      }
      else if(i == 5){
         mode = e__(height);
      }
      Roen::neoc("system/mode:" + mode.toString()).sect(-1)
         .sub("bigtree").act([=]{
            applyMode(mode, tree_perc_func);
         })--
         .sub("vegetation").act([=]{
            applyMode(mode, veg_perc);
         })--
         .sub("population").act([=]{
            applyMode(mode, pop_num);
         })--
         .sub("urbanheat").act([=]{
            applyMode(mode, urbanheat);
         })--
         .sub("grey").act([=]{
            applyMode(mode, greyer);
         })--
         .sub("random").act([=]{
            applyMode(mode, randomer);
         })--
         ;
   }}
   _keyceptor[0](rk_r) / [this]{
      heightMode([this](auto& mb, auto& sadata, auto& satwo){
         return floatrandbetween(0.0f, 0.3f); // let's try this
      });
      colorizeMode(0.8f, [this](auto& mb, auto& sadata, auto& satwo){
         return floatrandbetween(0.0f, 1.0f); // let's try this
      });
      return true;
   };
   _keyceptor[0](rk_a) / [this]{
      greenColorMode([this](auto& mb, auto& sadata, auto& satwo){
         return mb.bigtree_perc / 20.0f;
      });
      heightMode([this](auto& mb, auto& sadata, auto& satwo){
         return mb.bigtree_perc / 20.0f;
      });
      return true;
   };
   _keyceptor[0](rk_y) / [this]{
      current_age--;
      if(current_age < 0)
         current_age = 70;
      showAge(current_age);
      return true;
   };
   _keyceptor[0](rk_o) / [this]{
      current_age++;
      if(current_age > 70)
         current_age = 0;
      showAge(current_age);
      return true;
   };
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::showAge(int theage){
   redColorMode([this, theage](auto& mb, auto& sadata, auto& satwo){
      return (sadata.agedPop(theage * 4) + sadata.agedPop(theage * 4 + 1) + sadata.agedPop(theage * 4 + 2) + sadata.agedPop(theage * 4 + 3)) / 80.0f;
   });
   heightMode([this, theage](auto& mb, auto& sadata, auto& satwo){
      return (sadata.agedPop(theage * 4) + sadata.agedPop(theage * 4 + 1) + sadata.agedPop(theage * 4 + 2) + sadata.agedPop(theage * 4 + 3)) / 80.0f;
   });
   __openNoti("[v20]<196>Showing Age : <250>" + __toString(theage * 4) + " - " + __toString(theage * 4 + 3)).expireIn(1);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::initCanosi(){
   details_canosi
      .agentpos(10, 10, 190)
      .gus("blackblock")
      .layernum(30)
      .leftAlignText()
      .vis(has_focus_xnl)
      ++
         .save(detailscontainer)
      --
      ;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::maybeResyncMeshVBO(){
   if(mainrva_needresynmc){
      mainrva.unsyncVBO();
      mainrva.syncVBO();
      mainrva_needresynmc = false;
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::greenColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc){
   sloopmapex(meshies, eh, moo)
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color._green = leval;
      moo.recolor();
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::redColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc){
   sloopmapex(meshies, eh, moo)
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color._red = leval;
      moo.recolor();
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::blueColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc){
   sloopmapex(meshies, eh, moo)
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color._blue = leval;
      moo.recolor();
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::heightMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc){
   sloopmapex(meshies, eh, moo)
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_height = floatinterpolate(clampwithin(leval, 0, 1), 0.2f, 8.0f);
      moo.reheight();
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::colorizeMode(const ColorInfo& lecol, function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc){
   sloopmapex(meshies, eh, moo)
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color = ColorInfo(0.3f).interpolate(lecol, leval);
      moo.recolor();
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::applyMode(eostri mode, function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc){
   if(mode == e__(green)){
      greenColorMode(lefunc);
   }
   else if(mode == e__(red)){
      redColorMode(lefunc);
   }
   else if(mode == e__(blue)){
      blueColorMode(lefunc);
   }
   else if(mode == e__(height)){
      heightMode(lefunc);
   }
   else if(mode == e__(greenifier)){
      colorizeMode(22_xcoel, lefunc);
   }
   else if(mode == e__(grey)){
      colorizeMode(1.0f, lefunc);
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::initMeshies(){
   psf.loadFile(bunschfile("basicsydney.flec").path());
   int tringcount = 0;
   croust>>(33)<<"Record count ">>(250)<<(psf.records.size()) << ozendl;
   float area_max = 0.0908096;
   splaicht(cuboid_skip_right);
   splaicht(cuboid_skip_left);
   int smallcount = 0;
   int midcount = 0;
   int hugecount = 0;
   sloopvec(psf.records, moo)
      uint64_t mb_id = Morestring::toU64(moo.attribs[0]);
      auto& meshy = meshies[mb_id];
      FloatRect2D zto_ranges = psf.ztoise(moo.range);
      FloatRect2D scaled_ranges;
      scaled_ranges.topleft = localiseNSWPos(zto_ranges.topleft);
      scaled_ranges.botright = localiseNSWPos(zto_ranges.botright);
      meshy.rect = scaled_ranges;
      meshytree.insert(&meshy, meshy.rect);
      float actual_area = moo.range.area();
      float area_fac = actual_area / area_max;
      float chosen_height = floatrandbetween(0.5f, 3);
      meshy.main_height = chosen_height;
      if(area_fac < 0.001f){
         smallcount++;
      } else if (area_fac < 0.005f){
         midcount++;
      } else {
         hugecount++;
      }
      bool makefull = true;
      if(area_fac < 0.0005f){
      } else if(area_fac < 0.001f){
         makefull = moo.fillrate < 0.25f;
      } else if(area_fac < 0.002f){
         makefull = moo.fillrate < 0.45f;
      } else if(area_fac < 0.003f){
         makefull = moo.fillrate < 0.65f;
      } else if(area_fac < 0.004f){
         makefull = moo.fillrate < 0.75f;
      } else {
         makefull = true;
      }
      float standard_height = 1;
      meshy.mainrange.from = mainrva.poses.size();
      meshy.flatrange.from = flatrva.poses.size();
      ColorInfo somecol = floatrandbetween(0.3f, 0.8f);
      { switch((meshy.mb->reclass).onlyval){
         case e__(com).onlyval:
            {
            somecol = 196_xcoel;
            }; break;
         case e__(park).onlyval:
            {
            somecol = 112_xcoel;
            }; break;
         case e__(primprod).onlyval:
            {
            somecol = 22_xcoel;
            }; break;
         case e__(water).onlyval:
            {
            somecol = 33_xcoel;
            }; break;
      }}
      float vec_fac = (100 - meshy.mb->nonveg_perc) / 100.0f;
      float youth_fac = meshy.sa1data->youth_factor;
      somecol = ColorInfo(floatrandbetween(0.3f, 0.7f)).interpolate(196_xcoel, youth_fac);
      if(meshy.sa1data->malecount + meshy.sa1data->femalecount < 10 || meshy.mb->pop_2016 < 3){
         somecol = floatrandbetween(0.3f, 0.7f);
      } else {
         somecol = somecol; // let's try this
      }
      somecol = floatrandbetween(0.3f, 0.8f);
      meshy.main_color = somecol;
      {
         tringcount += moo.poly_indices.size() / 3;
         sloopvec(moo.poly_indices, leindex)
            mainrva.addVertex(Coord3D(localiseNSWPos(psf.ztoise(moo.points[leindex])), chosen_height), somecol, Coord3D(0, 0, 1));
            flatrva.addVertex(Coord3D(localiseNSWPos(psf.ztoise(moo.points[leindex])), standard_height));
         sloopend
         sloopveccounta(moo.points, lepoint, counta)
            Point2D ztome = localiseNSWPos(psf.ztoise(lepoint));
            Point2D ztoprev;
            if(counta == 0){
               ztoprev = localiseNSWPos(psf.ztoise(moo.points.back()));
            } else {
               ztoprev = localiseNSWPos(psf.ztoise(moo.points[counta - 1]));
            }
            Coord3D h1(ztome, chosen_height);
            Coord3D h2(ztoprev, chosen_height);
            Coord3D fh1(ztome, standard_height);
            Coord3D fh2(ztoprev, standard_height);
            Coord3D h3(ztoprev, 0);
            Coord3D h4(ztome, 0);
            mainrva.addSquarePolygonAutoNorm(h1, h2, h3, h4, somecol);
            flatrva.addSquarePolygon(fh1, fh2, h3, h4);
         sloopend
      }
      meshy.mainrange.to = mainrva.poses.size();
      meshy.flatrange.to = flatrva.poses.size();
   sloopend
   croust>>(196)<<"tringcount ">>(220)<<(tringcount) << ozendl;
   croust>>(196)<<"smallcount ">>(220)<<(smallcount) << ozendl;
   croust>>(196)<<"midcount ">>(220)<<(midcount) << ozendl;
   croust>>(196)<<"hugecount ">>(220)<<(hugecount) << ozendl;
   mainrva.syncVBO();
   flatrva.syncVBO();
   mainrva_needresynmc = false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool Game::handleTouchEvent(const TouchInfo& tinfoil){
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::setup(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::print(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::draw2D(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::draw3D(){
   BG::mk().drawGradientGround(0.7f, 0.6f, 0.9f, 1.0f);
   mmPush(); mmTranslate(0, 0, 0);{
      mainrva.drawTriangles();
      flatrva.smuldraw([this]{
         sloopset(activehovers, moo)
            moo->drawHoverSmulPart(flatrva);
         sloopend
      });
   } mmPop();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::update(){
   last_flatmouse = BG::marsiso().flatPosAtCurrentMousePos(0);
   FloatRect2D recter = FloatRect2D(last_flatmouse, 12);
   vector<Meshy*> meshies = meshytree.search(recter);
   sloopvec(meshies, moo)
      moo->activate();
   sloopend
   float closestdist = 999;;
   Meshy * newclosest = NULL;
   slooprem(activehovers, moo, remit)
      moo->update();
      if(moo->last_hover_dist < closestdist){
         newclosest = moo;
         closestdist = moo->last_hover_dist;
      }
   sloopend
   if(closesthover != newclosest)
      detail_focus_counter = 0;
   closesthover = newclosest;
   has_focus_xnl = newclosest != NULL && newclosest->hover_active;
   detail_focus_counter++;
   if(detail_focus_counter == 2){
      syncDetailFocus();
   }
   maybeResyncMeshVBO();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::syncDetailFocus(){
   if(last_detailed == closesthover){
      return;
   }
   last_detailed = closesthover;
   if(last_detailed){
      auto& mb = *last_detailed->mb;
      auto& postcode = *last_detailed->postcode;
      auto& sadata = *last_detailed->sa1data;
      detailscontainer->clearTexts();
      detailscontainer->txt(__toString(mb.postcode) + ", Sydney", "ma19", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("type : " + mb.reclass.toString(), "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("area : " + __toString(mb.area), "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("grass : " + __toString(mb.grass_perc) + "%", "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("shrub : " + __toString(mb.shrub_perc) + "%", "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("trees : " + __toString(mb.anytree_perc) + "%", "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("pop : " + __toString(mb.pop_2016), "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("dwellings : " + __toString(mb.dwellings_2016), "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("UHI : " + __toString(mb.uhi_index), "ma14", 1.0f);
      detailscontainer->newline();
      detailscontainer->txt("-----------------------------------", "ma14", 33_xcoel);
      detailscontainer->newline();
      int i;
      for(i = 0; i < 20; i++){
         int ages = sadata.agedPop(i * 4) + sadata.agedPop(i * 4 + 1) + sadata.agedPop(i * 4 + 2) + sadata.agedPop(i * 4 + 3);
         detailscontainer->txt("age " + __toString(i * 4) + " - " + __toString(i * 4 + 3) + " : " + __toString(ages), "s8", 1.0f);
         detailscontainer->newline();
      }
      detailscontainer->leftAlignText();
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::drawPostParticles(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::drawPostWave(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Game::handleMarsTap(const TouchInfo& tinfoil){
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
Meshy::Meshy(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Meshy::update(){
   last_hover_dist = rect.pos().lengthFrom(game->last_flatmouse);
   float liveness = signedpow((6 - last_hover_dist) / 6.0f, 5.5f);
   if(game->closesthover == this && game->closesthover){
      popness.chase2 = 1.0f;
      if(last_hover_dist > 6){
         popness.chase2 = 0;
      }
   }
   else {
      popness.chase2 = liveness;
   }
   popness.chaseByFraction(0.2f, __ldtf);
   if(popness.chase2 <= 0.01f && popness.val < 0.01f){
      deactivate();
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Meshy::recolor(){
   int i;
   for(i = mainrange.from; i < mainrange.to; i++){
      game->mainrva.colors[i] = main_color;
   }
   game->mainrva_needresynmc = true;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Meshy::reheight(){
   int i;
   for(i = mainrange.from; i < mainrange.to; i++){
      auto& lezpos = game->mainrva.poses[i].z;
      if(lezpos > 0.001f){
         lezpos = main_height;
      }
   }
   game->mainrva_needresynmc = true;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Meshy::deactivate(){
   if(!hover_active){
      return;
   }
   hover_active = false;
   game->activehovers.erase(this);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Meshy::activate(){
   if(hover_active){
      return;
   }
   hover_active = true;
   game->activehovers.insert(this);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Meshy::drawHoverSmulPart(RVA& bigrva){
   if(game->closesthover == this){
      bigrva.smulCol(196_xcoel);
   }
   else {
      bigrva.smulCol(main_color);
   }
   float scaleval = floatinterpolate(popness.val, main_height, 12.0f); // let's try this
   Matrix3D scaler;
   scaler.scale(1, 1, scaleval);
   bigrva.smulMat(scaler);
   bigrva.smulPart(GL_TRIANGLES, flatrange); // let's do this
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
