////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/fileparser.hpp"
#include "../shing/eostri.hpp"
#include "../shing/moremath.hpp"
#include "../shing/stats.hpp"
#include "shp_structs.hpp"
#include "../shing/classdef.hpp"
#include "shp.cl.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class FileSHPHeaderRanges{
   public:
   //-------------------------------------------------------
   double min_x = 0;
   double min_y = 0;
   double max_x = 0;
   double max_y = 0;
   double min_z = 0;
   double max_z = 0;
   double min_m = 0;
   double max_m = 0;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class FileSHPRec{
   public:
   //-------------------------------------------------------
   int record_num = 0;
   int record_len = 0;
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class Record{
   public:
   //-------------------------------------------------------
   int recnum = 0;
   int shapetype_num = 0;
   eostri shapetype;
   shringer contentrange;
   bool content_errored = false;
   double min_x = 0;
   double min_y = 0;
   double max_x = 0;
   double max_y = 0;
   FloatRect2D zto_ranges;
   vector<Point2D> points;
   vector<Point2D> ztopoints;
   vector<uint32_t> parts;
   vector<shringer> fieldvalues;
   float fillrate = 0;
   float original_area = 0;
   int max_reducted_size = 0;
   //-------------------------------------------------------
   void loadPolygon(ShapeFile& master);
   bool massCullStep(float max_percent_diff);
   bool neoCullStep(float max_percent_diff);
   void performNeoCulling(ShapeFile& master, float max_percent_diff);
   void cullPolygonEdges(ShapeFile& master, float dotnum);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class FieldDesc{
   public:
   //-------------------------------------------------------
   string name;
   char type = 0;
   int length = 0;
   int decimalcount = 0;
   set<uint64_t> possible_hashes;
   set<string> possible_values_collection;
   //-------------------------------------------------------
   void regPossibleValue(const shringer& lestr);
   void printSummary();
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
class ShapeFile{
   public:
   //-------------------------------------------------------
   Shing::FileParser shp_fp;
   Shing::FileParser dbf_fp;
   int32_t header_filecode = 0;
   int32_t header_filelen = 0;
   int32_t header_version = 0;
   int32_t header_shapetype_num = 0;
   eostri header_shapetype;
   vector<Record> records;
   set<eostri> encountered_shapetypes;
   set<int> unknown_types_encountered;
   uint8_t dbase_version_num = 0;
   string dbase_version_name;
   int dbase_record_bytes_total = 0;
   int dbase_record_count = 0;
   vector<FieldDesc> fielddescs;
   FileSHPHeaderRanges headerranges;
   int original_pointcount = 0;
   int postculled_pointcount = 0;
   FloatRect2D geo_range;
   //-------------------------------------------------------
   ShapeFile();
   Point2D ztoise(double x, double y);
   eostri shapeTypeName(int thenum);
   void syncDbaseVersionName();
   void loadDbf(const schfile& theschfile);
   void loadShp(const schfile& theschfile);
   void load(const schfile& theschfile);
   bool errored();
   void printErrors();
   void testPrint();
   void neoCullEdges(float max_perc_diff);
   void cullPolygonEdges(float dotnum);
   void filterContains(int fieldindex, const string& thefilter);
   void recalculateBounds();
   void filterStarts(int fieldindex, const string& thefilter);
   void colsToCSV(vector<int> columns);
   void determineFillRate();
   void saveTo(ProcessedShapeFile& outfile);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
