////////////////////////////////////////////////////////////
#include "meshblocks.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
MBHost::MBHost(){
   flector_defstart(2)
      flector_mapfield(postcodes);
      flector_mapfield(meshblocks);
      flector_mapfield(sadatas);
      flector_mapfield(satwos);
   flector_defend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void MBHost::saveFile(const string& thefile){
   flector_savefile(thefile);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void MBHost::loadFile(const string& thefile){
   flector_loadfile(thefile);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
MeshBlock& MBHost::ob(uint64_t theindex){
   auto it = meshblocks.find(theindex);
   asserts(it != meshblocks.end(), "Can't find meshblock " + __toString(theindex));
   return it->second;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void MBHost::loadRaw(){
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<62>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa2_raw_names.csv", true);
      sloopvec(csvfile, moo)
         sa2_names[moo[2].toString()] = Morestring::toInt(moo[1].toString());
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<74>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa2_walkers.csv", true);
      sloopvec(csvfile, moo)
         string lename = moo[0].toString();
         if(sa2_names.count(lename) == 0){
            oustoust[196] << "Missing sa2 name : " >> 250 << lename << ozendl;
         } else {
            asserts(sa2_names.count(lename) != 0, "Missing walker sa2.. " + lename);
            int sa2code = sa2_names[lename];
            auto& satwo = satwos[sa2code];
            satwo.walkers_count = Morestring::toInt(moo[1].toString());
            satwo.cyclers_count = Morestring::toInt(moo[2].toString());
            sa2_names[moo[2].toString()] = Morestring::toInt(moo[1].toString());
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<101>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa1_allages.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            uint32_t sacode = Morestring::toInt(moo[0].toString());
            vector<short> pops;
            EasyParser parsa(moo[1]);
            while(parsa.hasChar()){
               int thenum = parsa.parseNumber();
               pops.push_back(thenum);
               parsa << '|';
            }
            auto& saer = sadatas[sacode];
            saer.pops = pops;
            int totalsapop = 0;
            int under30 = 0;
            sloopveccounta(pops, moo, counta)
               if(counta <= 30){
                  under30 += moo;
               }
               totalsapop += moo;
            sloopend
            if(totalsapop > 0){
               saer.youth_factor = under30 / (float)totalsapop;
            } else {
               saer.youth_factor = 0;
            }
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<144>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/meshblock_uhi.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[1].toString(), "1")){
            uint64_t mbcode = Morestring::toU64(moo[0].toString());
            auto& mb = meshblocks[mbcode];
            EasyParser parsa(moo[1]);
            mb.uhi_index = parsa--.parseFloat();
            parsa << "e";
            if(parsa << "+001"){
               mb.uhi_index *= 10;
            }
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<169>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa1_male_female.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            uint32_t sacode = Morestring::toInt(moo[0].toString());
            auto& saer = sadatas[sacode];
            saer.malecount = Morestring::toInt(moo[1].toString());
            saer.femalecount = Morestring::toInt(moo[2].toString());
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<186>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/vegetation_cover/AttributeTableCSV/vecov.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[1].toString(), "1")){
            uint64_t mbcode = Morestring::toU64(moo[1].toString());
            auto& mb = meshblocks[mbcode];
            string reclasstype = moo[3].toString();
            if(reclasstype == "Commercial"){
               mb.reclass = e__(com);
            }
            else if(reclasstype == "Education"){
               mb.reclass = e__(edu);
            }
            else if(reclasstype == "Hospital/Medical"){
               mb.reclass = e__(medi);
            }
            else if(reclasstype == "Industrial"){
               mb.reclass = e__(indus);
            }
            else if(reclasstype == "Infrastructure"){
               mb.reclass = e__(infra);
            }
            else if(reclasstype == "Other"){
               mb.reclass = e__(other);
            }
            else if(reclasstype == "Parkland"){
               mb.reclass = e__(park);
            }
            else if(reclasstype == "Primary Production"){
               mb.reclass = e__(primprod);
            }
            else if(reclasstype == "Residential"){
               mb.reclass = e__(resi);
            }
            else if(reclasstype == "Transport"){
               mb.reclass = e__(trans);
            }
            else if(reclasstype == "Water"){
               mb.reclass = e__(water);
            }
            else {
               asserts(0, "unhandled " + reclasstype); // wut
            }
            if(moo[4] == "Public"){
               mb.ispublic = true;
            }
            else if(moo[4] == "Private"){
               mb.isprivate = true;
            }
            mb.area = Morestring::toFloat(moo[9].toString());
            mb.grass_perc = Morestring::toFloat(moo[19].toString());
            mb.shrub_perc = Morestring::toFloat(moo[20].toString());
            mb.anytree_perc = Morestring::toFloat(moo[24].toString());
            mb.smalltree_perc = Morestring::toFloat(moo[21].toString());
            mb.midtree_perc = Morestring::toFloat(moo[22].toString());
            mb.bigtree_perc = Morestring::toFloat(moo[23].toString());
            mb.nonveg_perc = Morestring::toFloat(moo[27].toString());
            mb.sa1_code = Morestring::toInt(moo[6].toString());
            asserts(sadatas.count(mb.sa1_code) != 0, "No sadata : " + __toString(mb.sa1_code) + "??");
            mb.sa2_code = Morestring::toInt(moo[8].toString());
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<249>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/POA_2016_AUST.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            int postcode = Morestring::toInt(moo[2].toString());
            uint64_t mbcode = Morestring::toU64(moo[0].toString());
            meshblocks[mbcode].postcode = postcode;
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<267>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/australian_postcodes.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "2")){
            int postcode = Morestring::toInt(moo[0].toString());
            postcodes[postcode].addName(moo[1].toString());
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<283>>240<<']'>>250<<' '>>(33)<<" -------- ! " << ozendl;
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/mb_pop_2016.csv", true);
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            uint64_t mbcode = Morestring::toU64(moo[0].toString());
            auto& mb = meshblocks[mbcode];
            mb.dwellings_2016 = Morestring::toInt(moo[3].toString());
            mb.pop_2016 = Morestring::toInt(moo[4].toString());
         }
      sloopend
   }
   croust>>240<<'['>>235<<"nd:cj/">>106 <<"MBHost">>100<<'/'>>110 <<"loadRaw">>238<<":">>235<<304>>240<<']'>>250<<' '>>(33)<<" -------- end! " << ozendl;
   croust>>(33)<<"MBs loaded ">>(250)<<(meshblocks.size()) << ozendl;
   croust>>(250)<<"postcodes ">>(240)<<(postcodes.size()) << ozendl;
   croust>>(250)<<"sadatas ">>(240)<<(sadatas.size()) << ozendl;
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
MeshBlock::MeshBlock(){
   initflec();
}
////////////////////////////////////////////////////////////
void MeshBlock::initflec(){
   flector_defstart(2)
      flector_field(code16).fromVersion(2);
      flector_field(reclass).fromVersion(2);
      flector_field(ispublic).fromVersion(2);
      flector_field(isprivate).fromVersion(2);
      flector_field(area).fromVersion(2);
      flector_field(grass_perc).fromVersion(2);
      flector_field(shrub_perc).fromVersion(2);
      flector_field(anytree_perc).fromVersion(2);
      flector_field(smalltree_perc).fromVersion(2);
      flector_field(midtree_perc).fromVersion(2);
      flector_field(bigtree_perc).fromVersion(2);
      flector_field(nonveg_perc).fromVersion(2);
      flector_field(sa1_code).fromVersion(2);
      flector_field(sa2_code).fromVersion(2);
      flector_field(sa3_code).fromVersion(2);
      flector_field(postcode).fromVersion(2);
      flector_field(pop_2016).fromVersion(2);
      flector_field(dwellings_2016).fromVersion(2);
      flector_field(uhi_index).fromVersion(2);
   flector_defend
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
PostCode::PostCode(){
   initflec();
}
////////////////////////////////////////////////////////////
void PostCode::initflec(){
   flector_defstart(2)
      flector_vecfield(possible_names).fromVersion(2);
   flector_defend
}
////////////////////////////////////////////////////////////
void PostCode::addName(const string& thestr){
   string touse = thestr;
   if(Morestring::beginsWith(thestr, "\"") && Morestring::endsWith(thestr, "\"")){
      touse = thestr.substr(1, thestr.size() - 2);
   }
   sloopvec(possible_names, moo)
      if(touse == moo){
         return;
      }
   sloopend
   possible_names.push_back(touse);
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
SA1Data::SA1Data(){
   initflec();
}
////////////////////////////////////////////////////////////
void SA1Data::initflec(){
   flector_defstart(2)
      flector_vecfield(pops).fromVersion(2);
      flector_field(malecount).fromVersion(2);
      flector_field(femalecount).fromVersion(2);
      flector_field(youth_factor).fromVersion(2);
   flector_defend
}
////////////////////////////////////////////////////////////
int SA1Data::agedPop(int agenum){
   if(agenum >= pops.size())
      return 0;
   return pops[agenum];
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
SA2Data::SA2Data(){
   initflec();
}
////////////////////////////////////////////////////////////
void SA2Data::initflec(){
   flector_defstart(2)
      flector_field(walkers_count).fromVersion(2);
      flector_field(cyclers_count).fromVersion(2);
   flector_defend
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
////////////////////////////////////////////////////////////
