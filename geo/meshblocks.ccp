//////////////////////////////////////////////////////////////////////////////////////////
#include flector shing.csvfile
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
PostCode/addName(const string& thestr)
   string touse = thestr;
   \if Morestring::beginsWith(thestr, "\"") && Morestring::endsWith(thestr, "\"")
      touse = thestr.substr(1, thestr.size() - 2);
   
   \sloop possible_names
      \if touse == moo
         return;
      
   possible_names.push_back(touse);

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
SA1Data/agedPop(int agenum) -> int
   if(agenum >= pops.size())
      return 0;
   
   return pops[agenum];
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
MBHost/MBHost()
   
   flector_defstart(2)
      
      flector_mapfield(postcodes);
      flector_mapfield(meshblocks);
      flector_mapfield(sadatas);
      flector_mapfield(satwos);
      
      // let's just do NSW for now eh?
      
   flector_defend
   
//////////////////////////////////////////////////////////////////////////////////////////
MBHost/saveFile(const string& thefile)
   flector_savefile(thefile);
//////////////////////////////////////////////////////////////////////////////////////////
MBHost/loadFile(const string& thefile)
   flector_loadfile(thefile);
//////////////////////////////////////////////////////////////////////////////////////////
MBHost/ob(uint64_t theindex) -> MeshBlock&
   auto it = meshblocks.find(theindex);
   asserts(it != meshblocks.end(), "Can't find meshblock " + __toString(theindex));
   return it->second;
//////////////////////////////////////////////////////////////////////////////////////////
MBHost/loadRaw()

   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa2_raw_names.csv", true);
      
      sloopvec(csvfile, moo)
         sa2_names[moo[2].toString()] = Morestring::toInt(moo[1].toString());
      sloopend
      
   }
   
   \\:: [33] " -------- ! "
   
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
   
   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa1_allages.csv", true);
      
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            // it's a new south wales!
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
   
   \\:: [33] " -------- ! "
   
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
   
   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/sa1_male_female.csv", true);
      
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            // it's a new south wales!
            uint32_t sacode = Morestring::toInt(moo[0].toString());
            auto& saer = sadatas[sacode];
            saer.malecount = Morestring::toInt(moo[1].toString());
            saer.femalecount = Morestring::toInt(moo[2].toString());
         }
      sloopend
   }
   
   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/vegetation_cover/AttributeTableCSV/vecov.csv", true);
      
      sloopvec(csvfile, moo)
         
         if(Morestring::beginsWith(moo[1].toString(), "1")){
            uint64_t mbcode = Morestring::toU64(moo[1].toString());
            
            auto& mb = meshblocks[mbcode];
            
            string reclasstype = moo[3].toString();
            
            \if reclasstype == "Commercial"
               mb.reclass = `\com;
            \elif reclasstype == "Education"
               mb.reclass = `\edu;
            \elif reclasstype == "Hospital/Medical"
               mb.reclass = `\medi;
            \elif reclasstype == "Industrial"
               mb.reclass = `\indus;
            \elif reclasstype == "Infrastructure"
               mb.reclass = `\infra;
            \elif reclasstype == "Other"
               mb.reclass = `\other;
            \elif reclasstype == "Parkland"
               mb.reclass = `\park;
            \elif reclasstype == "Primary Production"
               mb.reclass = `\primprod;
            \elif reclasstype == "Residential"
               mb.reclass = `\resi;
            \elif reclasstype == "Transport"
               mb.reclass = `\trans;
            \elif reclasstype == "Water"
               mb.reclass = `\water;
            \else
               asserts(0, "unhandled " + reclasstype); // wut
            
            \if moo[4] == "Public"
               mb.ispublic = true;
            \elif moo[4] == "Private"
               mb.isprivate = true;
            
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
            //asserts(satwos.count(mb.sa2_code) != 0, "No sa2 : " + __toString(mb.sa2_code) + "??");
         }
      sloopend
   }
   
   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/POA_2016_AUST.csv", true);
      
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            // it's a new south wales!
            int postcode = Morestring::toInt(moo[2].toString());
            uint64_t mbcode = Morestring::toU64(moo[0].toString());
            
            meshblocks[mbcode].postcode = postcode;
            
         }
      sloopend
   }
   
   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/australian_postcodes.csv", true);
      
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "2")){
            // it's a new south wales!
            int postcode = Morestring::toInt(moo[0].toString());
            postcodes[postcode].addName(moo[1].toString());
            
         }
      sloopend
   }
   
   \\:: [33] " -------- ! "
   
   {
      Shing::CSVFile csvfile;
      csvfile.load("/home/yose/govhack/mb_pop_2016.csv", true);
      
      sloopvec(csvfile, moo)
         if(Morestring::beginsWith(moo[0], "1")){
            // it's a new south wales!
            uint64_t mbcode = Morestring::toU64(moo[0].toString());
            
            auto& mb = meshblocks[mbcode];
            mb.dwellings_2016 = Morestring::toInt(moo[3].toString());
            mb.pop_2016 = Morestring::toInt(moo[4].toString());
            
         }
      sloopend
   }
   

   
   \\:: [33] " -------- end! "
   
   
   \\/[33] "MBs loaded " [250] meshblocks.size()
   \\/ [250] "postcodes " [240] postcodes.size()
   \\/ [250] "sadatas " [240] sadatas.size()
   
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
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
