//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#include roengl.touch roengl.roengl shing.rva
#bodinc roengl.utils threedifier shing.quickbenchmarker fixedmath muldraw roengl.uniformbuffer
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc basegame neocontext keyceptor neocommando secsy ncgl.gunlogic ncgl.anode
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc "../geo/shp.cl.hpp"
#bodinc "../include/earcut.hpp"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/Meshy()
   
   
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/update()
   last_hover_dist = rect.pos().lengthFrom(game->last_flatmouse);
   float liveness = signedpow((6 - last_hover_dist) / 6.0f, 5.5f);
   
   \if game->closesthover == this && game->closesthover
      popness.chase2 = 1.0f;
      \if last_hover_dist > 6
         popness.chase2 = 0;
   \else
      popness.chase2 = liveness;
      
   popness.chaseByFraction(0.2f, __ldtf);
   
   if(popness.chase2 <= 0.01f && popness.val < 0.01f){
      deactivate();
   }
   
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/recolor()
   int i;
   for(i = mainrange.from; i < mainrange.to; i++){
      game->mainrva.colors[i] = main_color;
   }
   game->mainrva_needresynmc = true;
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/reheight()
   int i;
   for(i = mainrange.from; i < mainrange.to; i++){
      auto& lezpos = game->mainrva.poses[i].z;
      if(lezpos > 0.001f){
         lezpos = main_height;
      }
   }
   game->mainrva_needresynmc = true;
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/deactivate()
   \if !hover_active
      return;
   
   hover_active = false;
   game->activehovers.erase(this);
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/activate()
   \if hover_active
      return;
   
   hover_active = true;
   game->activehovers.insert(this);
//////////////////////////////////////////////////////////////////////////////////////////
Meshy/drawHoverSmulPart(RVA& bigrva)
   
   \if game->closesthover == this
      bigrva.smulCol(196_xcoel);
   \else
      bigrva.smulCol(main_color);
   
   float scaleval = floatinterpolate(popness.val, main_height, 12.0f); // let's try this
   Matrix3D scaler;
   scaler.scale(1, 1, scaleval);
   bigrva.smulMat(scaler);
   bigrva.smulPart(GL_TRIANGLES, flatrange); // let's do this
   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Game/localiseNSWPos(const Point2D& lepos) -> Point2D
   return Point2D(lepos.x * 1200, lepos.y * -1000);
//////////////////////////////////////////////////////////////////////////////////////////
Game/Game()



   
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

   
//////////////////////////////////////////////////////////////////////////////////////////
Game/initColorizers()
   
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
   
   \looprange i [0..6)
      
      eostri mode;
      \if i == 0
         mode = `\red;
      \elif i == 1
         mode = `\green;
      \elif i == 2
         mode = `\blue;
      \elif i == 3
         mode = `\greenifier;
      \elif i == 4
         mode = `\grey;
      \elif i == 5
         mode = `\height;
      
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
   
   
   _keyceptor[0](rk_r) / [this]{
      // reset
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
   
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/showAge(int theage)
   redColorMode([this, theage](auto& mb, auto& sadata, auto& satwo){
      return (sadata.agedPop(theage * 4) + sadata.agedPop(theage * 4 + 1) + sadata.agedPop(theage * 4 + 2) + sadata.agedPop(theage * 4 + 3)) / 80.0f;
   });
   
   heightMode([this, theage](auto& mb, auto& sadata, auto& satwo){
      return (sadata.agedPop(theage * 4) + sadata.agedPop(theage * 4 + 1) + sadata.agedPop(theage * 4 + 2) + sadata.agedPop(theage * 4 + 3)) / 80.0f;
   });
   
   __openNoti("[v20]<196>Showing Age : <250>" + __toString(theage * 4) + " - " + __toString(theage * 4 + 3)).expireIn(1);
//////////////////////////////////////////////////////////////////////////////////////////
Game/initCanosi()
   
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
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/maybeResyncMeshVBO()
   
   \if mainrva_needresynmc
      mainrva.unsyncVBO();
      mainrva.syncVBO();
      mainrva_needresynmc = false;
   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Game/greenColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc)
   \sloopmap meshies
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color._green = leval;
      moo.recolor();
//////////////////////////////////////////////////////////////////////////////////////////
Game/redColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc)
   \sloopmap meshies
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color._red = leval;
      moo.recolor();
//////////////////////////////////////////////////////////////////////////////////////////
Game/blueColorMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc)
   \sloopmap meshies
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color._blue = leval;
      moo.recolor();
//////////////////////////////////////////////////////////////////////////////////////////
Game/heightMode(function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc)
   \sloopmap meshies
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_height = floatinterpolate(clampwithin(leval, 0, 1), 0.2f, 8.0f);
      moo.reheight();
//////////////////////////////////////////////////////////////////////////////////////////
Game/colorizeMode(const ColorInfo& lecol, function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc)
   \sloopmap meshies
      SA2Data noel;
      float leval = lefunc(*moo.mb, *moo.sa1data, noel);
      moo.main_color = ColorInfo(0.3f).interpolate(lecol, leval);
      moo.recolor();
//////////////////////////////////////////////////////////////////////////////////////////
Game/applyMode(eostri mode, function<float(MeshBlock&, SA1Data&, SA2Data&)> lefunc)
   
   \if mode == `\green
      greenColorMode(lefunc);
   \elif mode == `\red
      redColorMode(lefunc);
   \elif mode == `\blue
      blueColorMode(lefunc);
   \elif mode == `\height
      heightMode(lefunc);
   \elif mode == `\greenifier
      colorizeMode(22_xcoel, lefunc);
   \elif mode == `\grey
      colorizeMode(1.0f, lefunc);
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/initMeshies()

   
   //shp.load("/home/yose/govhack/nsw_suburbs/NSW_LOCALITY_POLYGON_shp.shp"); // let's do it
   //shp.load("/home/yose/govhack/nsw_meshblocks/MB_2016_NSW.shp"); // let's do it
   //shp.load("/home/yose/govhack/SA1map/SA1_2016_AUST.shp"); // let's do it
   //shp.load("/home/yose/govhack/SA2map/SA2_2016_AUST.shp"); // let's do it
   //psf.loadFile("/home/yose/govhack/temp.geo");
   psf.loadFile(bunschfile("basicsydney.flec").path());
   
   int tringcount = 0;
   
   \\/ [33] "Record count " [250] psf.records.size()
   
   float area_max = 0.0908096;
   
   
   //splaicht(cuboid_skip_bot);
   
   splaicht(cuboid_skip_right);
   splaicht(cuboid_skip_left);
   
   //splaicht(cuboid_skip_back);
   //splaicht(cuboid_skip_front);
   
   int smallcount = 0;
   int midcount = 0;
   int hugecount = 0;
   
   \sloop psf.records
      /*
      Point2D topleft = moo.zto_ranges.pos(0,0);
      Point2D topright = moo.zto_ranges.pos(1,0);
      Point2D botright = moo.zto_ranges.pos(1,1);
      Point2D botleft = moo.zto_ranges.pos(0,1);
      */
      
      //ColorInfo somecol = ColorInfo::numberedColor(intrandbetween(0, 20));
      
      //if(Morestring::beginsWith(moo.attribs[0], "1")){
      
      uint64_t mb_id = Morestring::toU64(moo.attribs[0]);
      auto& meshy = meshies[mb_id];
      
      FloatRect2D zto_ranges = psf.ztoise(moo.range);
      
      FloatRect2D scaled_ranges;
      scaled_ranges.topleft = localiseNSWPos(zto_ranges.topleft);
      scaled_ranges.botright = localiseNSWPos(zto_ranges.botright);
      meshy.rect = scaled_ranges;
      
      meshytree.insert(&meshy, meshy.rect);
      // yeah
      
      float actual_area = moo.range.area();
      float area_fac = actual_area / area_max;
      
      float chosen_height = floatrandbetween(0.5f, 3);
      //float chosen_height = floatinterpolate(meshy.sa1data->agedPop(25) / 40.0f, 0.5f, 2.0f);
      
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
         // don't make full
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
      
      // make full for all
      float standard_height = 1;
      
      meshy.mainrange.from = mainrva.poses.size();
      meshy.flatrange.from = flatrva.poses.size();
      
      ColorInfo somecol = floatrandbetween(0.3f, 0.8f);
      
      \eostriswitch meshy.mb->reclass
         \case `\com
            somecol = 196_xcoel;
         \case `\park
            somecol = 112_xcoel;
         \case `\primprod
            somecol = 22_xcoel;
         \case `\water
            somecol = 33_xcoel;
      
      float vec_fac = (100 - meshy.mb->nonveg_perc) / 100.0f;
      //somecol = ColorInfo(floatrandbetween(0.3f, 0.7f)).interpolate(112_xcoel, vec_fac);
      // let's see youth fac
      
      
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
      
      /*
      sloopvec(moo.poly_indices, leindex)
         //mainrva.addVertex(Coord3D(moo.points[leindex], chosen_height), somecol);
      sloopend
      */
      
      
      
      /*
      using Epoint = std::array<float, 2>;
         
      vector<vector<Epoint>> tespoints; // for earcut
      tespoints.push_back(vector<Epoint>()); 
      
      sloopvec(moo.ztopoints, lepoint)
         tespoints.back().push_back({lepoint.x, lepoint.y});
      sloopend
      
      vector<int> indices = mapbox::earcut<int>(tespoints);
      
      auto& vecfirst = tespoints.front();
      sloopvec(indices, moo)
         //geomrva.addVertex(Point2D(vecfirst[moo][0], vecfirst[moo][1]));
      sloopend
      
      tringcount += indices.size() / 3;
      */
      
   
   \\/[196] "tringcount " [220] tringcount
   \\/[196] "smallcount " [220] smallcount
   \\/[196] "midcount " [220] midcount
   \\/[196] "hugecount " [220] hugecount
   
   mainrva.syncVBO();
   flatrva.syncVBO();
   mainrva_needresynmc = false;
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/handleTouchEvent(const TouchInfo& tinfoil) -> bool
   return false;
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/setup()
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/print()
   // nothing for now
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/draw2D()
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/draw3D()

   BG::mk().drawGradientGround(0.7f, 0.6f, 0.9f, 1.0f);
   
   \mmpush 0, 0, 0
      
      mainrva.drawTriangles();
      
      // now, draw the active hovers, let's try
      
      flatrva.smuldraw([this]{
         
         sloopset(activehovers, moo)
            moo->drawHoverSmulPart(flatrva);
         sloopend
         
      });
   
   // load them shapefile
   
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/update()
   
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
   \if detail_focus_counter == 2
      syncDetailFocus();
   
   maybeResyncMeshVBO();
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/syncDetailFocus()
   \if last_detailed == closesthover
      return;
   last_detailed = closesthover;
   
   \if last_detailed
      
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
      
      
      // okay, age pops
      // here here
      
      
      detailscontainer->leftAlignText();
      
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/drawPostParticles()
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/drawPostWave()
   
//////////////////////////////////////////////////////////////////////////////////////////
Game/handleMarsTap(const TouchInfo& tinfoil)
   
   
   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
