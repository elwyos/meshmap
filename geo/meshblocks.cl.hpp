////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/eostri.hpp"
#include "../shing/flector.hpp"
#include "../shing/csvfile.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class PostCode{
   public:
   //-------------------------------------------------------
   vector<string> possible_names;
   //-------------------------------------------------------
   flector_subbie_decl;
   PostCode();
   void initflec();
   void addName(const string& thestr);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class SA1Data{
   public:
   //-------------------------------------------------------
   vector<short> pops;
   int malecount = 0;
   int femalecount = 0;
   float youth_factor = 0;
   //-------------------------------------------------------
   flector_subbie_decl;
   SA1Data();
   void initflec();
   int agedPop(int agenum);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class SA2Data{
   public:
   //-------------------------------------------------------
   float walkers_count = 0;
   float cyclers_count = 0;
   //-------------------------------------------------------
   flector_subbie_decl;
   SA2Data();
   void initflec();
};
////////////////////////////////////////////////////////////
class MeshBlock{
   public:
   //-------------------------------------------------------
   uint64_t code16 = 0;
   eostri reclass;
   bool ispublic = false;
   bool isprivate = false;
   float area = 0;
   float grass_perc = 0;
   float shrub_perc = 0;
   float anytree_perc = 0;
   float smalltree_perc = 0;
   float midtree_perc = 0;
   float bigtree_perc = 0;
   float nonveg_perc = 0;
   uint32_t sa1_code = 0;
   uint32_t sa2_code = 0;
   uint32_t sa3_code = 0;
   int32_t postcode = 0;
   uint32_t pop_2016 = 0;
   uint32_t dwellings_2016 = 0;
   float uhi_index = 0;
   //-------------------------------------------------------
   flector_subbie_decl;
   MeshBlock();
   void initflec();
};
////////////////////////////////////////////////////////////
class MBHost{
   public:
   //-------------------------------------------------------
   map<string,uint32_t> sa2_names;
   map<string,string> attribs;
   map<uint32_t,SA1Data> sadatas;
   map<uint32_t,SA2Data> satwos;
   map<uint32_t,PostCode> postcodes;
   map<uint64_t,MeshBlock> meshblocks;
   //-------------------------------------------------------
   MBHost();
   void saveFile(const string& thefile);
   void loadFile(const string& thefile);
   MeshBlock& ob(uint64_t theindex);
   void loadRaw();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
