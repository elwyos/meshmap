////////////////////////////////////////////////////////////
#include "processed_shape.cl.hpp"
#include "../shing/schfile.hpp"
#include "shp.cl.hpp"
#include "../shing/endian.hpp"
#include "../shing/morestring.hpp"
#include "../shing/neocontext.hpp"
#include "../shing/curvemapper.hpp"
#include <algorithm>
#include "../include/earcut.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
float determineArea(vector<Point2D>& vecca);
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
void FieldDesc::regPossibleValue(const shringer& lestr){
   possible_hashes.insert(Morestring::hash3(lestr));
   if(possible_values_collection.size() >= 20){
      return;
   }
   possible_values_collection.insert(__chompends(lestr.toString()));
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void FieldDesc::printSummary(){
   croust>>(33)<<(name )<<" ">>(250)<<((char)type)>>(240)<<" - "<<(length )<<" bytes" << ozendl;
   if(possible_values_collection.size() != 0){
      oustoust[33] << "possible_hashes : " >> 250 << possible_hashes.size() << ozendl;
      oustoust.push(3);
         sloopvec(possible_values_collection, moo)
            if(moo == ""){
               croust>>(196)<<"\"\"">>(100)<<", ";
            }
            else {
               croust>>(244)<<(moo )>>(100)<<", ";
            }
         sloopend
         croust << ozendl;
      oustoust.pop();
   }
   else {
   }
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
void Record::loadPolygon(ShapeFile& master){
   if(shapetype != e__(polygon)){
      return;
   }
   EasyParser parsa(contentrange);
   if(!parsa.absorbDouble(min_x)){
      content_errored = true; return;
   }
   if(!parsa.absorbDouble(min_y)){
      content_errored = true; return;
   }
   if(!parsa.absorbDouble(max_x)){
      content_errored = true; return;
   }
   if(!parsa.absorbDouble(max_y)){
      content_errored = true; return;
   }
   zto_ranges.topleft = master.ztoise(min_x, min_y);
   zto_ranges.botright = master.ztoise(max_x, max_y);
   uint32_t partcount = 0;
   uint32_t pointcount = 0;
   if(!parsa.absorbInt(partcount)){
      content_errored = true; return;
   }
   if(!parsa.absorbInt(pointcount)){
      content_errored = true; return;
   }
   { int _rangetotal = partcount;
   for(int i = 0; i < _rangetotal; i++){
      uint32_t somepart = 0;
      if(!parsa.absorbInt(somepart)){
         content_errored = true; return;
      }
      parts.push_back(somepart);
   }}
   { int _rangetotal = pointcount;
   for(int i = 0; i < _rangetotal; i++){
      double pointx = 0;
      double pointy = 0;
      if(!parsa.absorbDouble(pointx)){
         content_errored = true; return;
      }
      if(!parsa.absorbDouble(pointy)){
         content_errored = true; return;
      }
      if(i == 0){
         min_x = max_x = pointx;
         min_y = max_y = pointy;
      }
      else {
         min_x = min(min_x, pointx);
         min_y = min(min_y, pointy);
         max_x = max(max_x, pointx);
         max_y = max(max_y, pointy);
      }
      points.push_back(Point2D(pointx, pointy));
   }}
   sloopvec(points, moo)
      ztopoints.push_back(master.ztoise(moo.x, moo.y));
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool Record::massCullStep(float max_percent_diff){
   if(points.size() <= max_reducted_size){
      return false; // do these ones with neo
   }
   static Shing::Random srando;
   float best_diff = max_percent_diff;
   vector<Point2D> cutcand;
   { int _rangetotal = 30;
   for(int phase = 0; phase < _rangetotal; phase++){
      vector<int> indices;
      { int _rangetotal = points.size();
      for(int i = 0; i < _rangetotal; i++){
         indices.push_back(i);
      }}
      srando.shuffle(indices);
      int toletlive = 95 * indices.size() / 100;
      indices.resize(toletlive);
      sort(indices.begin(), indices.end());
      vector<Point2D> newpoints;
      sloopvec(indices, leindex)
         newpoints.push_back(points[leindex]);
      sloopend
      float newarea = determineArea(newpoints);
      float fac = 100 * (newarea / original_area);
      float difference = fabs(fac - 100);
      if(difference < best_diff){
         best_diff = difference;
         cutcand = newpoints;
      }
   }}
   if(cutcand.size() != 0){
      points = cutcand;
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool Record::neoCullStep(float max_percent_diff){
   if(points.size() <= max_reducted_size){
      return false;
   }
   int pointcount = points.size();
   int best_index = -1;
   float best_cull_change = max_percent_diff; // 5 is max
   { int _rangetotal = pointcount;
   for(int i = 0; i < _rangetotal; i++){
      vector<Point2D> copia = points;
      Point2D tocut = points[i];
      copia.erase(copia.begin() + i); // try this one for size
      if(i == points.size()){
         points[0] = points[0].interpolate(tocut, 0.5f);
      } else {
         points[i] = points[i].interpolate(tocut, 0.5f);
      }
      float thisarea = determineArea(copia);
      float fac = 100 * (thisarea / original_area);
      float difference = fabs(fac - 100);
      if(difference < best_cull_change){
         best_index = i;
         best_cull_change = difference;
      }
   }}
   if(best_index != -1){
      points.erase(points.begin() + best_index); // yeah, boy
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Record::performNeoCulling(ShapeFile& master, float max_percent_diff){
   master.original_pointcount += points.size();
   original_area = determineArea(points);
   definecurve(area_minner, (500, 40)
                            (100, 10)
                            (50, 4));
   max_reducted_size = area_minner.get(points.size());
   bool printit = points.size() > 300;
   if(printit){
      cout << "Performing neoculling... : " << points.size() << endl;
   }
   splaicht(neoculling);
   cullPolygonEdges(master, 0.76f); // let's try this
   if(printit){
      cout << "New points :" << points.size() << endl;
   }
   while(true){
      bool succ = massCullStep(max_percent_diff);
      if(!succ){
         break;
      }
   }
   if(printit){
      cout << "post masscull : " << points.size() << endl;
   }
   while(true){
      bool succ = neoCullStep(max_percent_diff);
      if(!succ){
         break;
      }
   }
   if(printit){
      cout << "Postcull size : " << points.size() << endl;
      cout << "------------" << endl;
   }
   master.postculled_pointcount += points.size();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Record::cullPolygonEdges(ShapeFile& master, float dotnum){
   gplaicht(neoculling);
   if(!neoculling)
      master.original_pointcount += points.size();
   vector<Point2D> postculled = points;
   int curindex = 0;
   while(curindex + 2 < postculled.size()){
      Point2D starting_diff = (postculled[curindex + 1] - postculled[curindex]).normalise();
      while(curindex + 2 < postculled.size()){
         Point2D next_diff = (postculled[curindex + 2] - postculled[curindex + 1]).normalise();
         float thedot = dot(next_diff, starting_diff);
         if(thedot > dotnum){
            postculled.erase(postculled.begin() + (curindex + 1)); // remove the corner
         } else {
            curindex++;
            break; // get to the otuer loop
         }
      }
   }
   points = postculled;
   if(!neoculling)
      master.postculled_pointcount += points.size();
   ztopoints.clear();
   sloopvec(points, moo)
      ztopoints.push_back(master.ztoise(moo.x, moo.y));
   sloopend
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
ShapeFile::ShapeFile(){
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
Point2D ShapeFile::ztoise(double x, double y){
   Point2D ret;
   ret.x = (x - headerranges.min_x) / (headerranges.max_x - headerranges.min_x);
   ret.y = (y - headerranges.min_y) / (headerranges.max_y - headerranges.min_y);
   return ret;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
eostri ShapeFile::shapeTypeName(int thenum){
   { switch(thenum){
      case 0:
         {
         return e__(nullshape);
         }; break;
      case 1:
         {
         return e__(point);
         }; break;
      case 3:
         {
         return e__(polyline);
         }; break;
      case 5:
         {
         return e__(polygon);
         }; break;
      case 8:
         {
         return e__(multipoint);
         }; break;
      case 11:
         {
         return e__(pointz);
         }; break;
      case 13:
         {
         return e__(polylinez);
         }; break;
      case 15:
         {
         return e__(polygonz);
         }; break;
      default: {
         return e__(unhandled);
         }; break;
   }}
   return e__(whut);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::syncDbaseVersionName(){
   { switch(dbase_version_num){
      case 0x02:
         {
         dbase_version_name = "FoxBase 1.0";
         }; break;
      case 0x03:
         {
         dbase_version_name = "FoxBase 2.x / dBASE III";
         }; break;
      case 0x83:
         {
         dbase_version_name = "FoxBase 2.x / dBASE III with memo file";
         }; break;
      case 0x30:
         {
         dbase_version_name = "Visual FoxPro";
         }; break;
      case 0x31:
         {
         dbase_version_name = "Visual FoxPro with auto increment";
         }; break;
      case 0x32:
         {
         dbase_version_name = "Visual FoxPro with varchar/varbinary";
         }; break;
      case 0x43:
         {
         dbase_version_name = "dBASE IV SQL Table, no memo file";
         }; break;
      case 0x63:
         {
         dbase_version_name = "dBASE IV SQL System, no memo file";
         }; break;
      case 0x8b:
         {
         dbase_version_name = "dBASE IV with memo file";
         }; break;
      case 0xcb:
         {
         dbase_version_name = "dBASE IV SQL Table with memo file";
         }; break;
      case 0xfb:
         {
         dbase_version_name = "FoxPro 2";
         }; break;
      case 0xf5:
         {
         dbase_version_name = "FoxPro 2 with memo file";
         }; break;
      default: {
         dbase_version_name = "unknown version name";
         }; break;
   }}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::loadDbf(const schfile& theschfile){
   auto& fp = dbf_fp;
   fp.init(theschfile);
   croust>>(220)<<"--------- DBF Loading ----------" << ozendl;
   croust>>(220)<<"file size ">>(250)<<(fp.membuf.size()) << ozendl;
   if(fp.errored){
      return;
   }
   uint8_t versionbyte = fp.parse8("versionbyte");
   dbase_version_num = versionbyte;
   syncDbaseVersionName();
   croust>>(33)<<"dbase version name : ">>(250)<<(dbase_version_name) << ozendl;
   string lastupdateformat = fp.parseBytes(3, "lastupdateformat");
   uint32_t rec_count = fp.parse32("rec_count");
   uint16_t header_bytes = fp.parse16("header_bytes");
   uint16_t record_bytes = fp.parse16("record_bytes");
   fp.parseBytes(3, "reserved");
   fp.parseBytes(13, "more reserved");
   fp.parseBytes(4, "even more reserved");
   int fielddesc_start = fp.currentPos();
   croust>>(240)<<"Header_bytes ">>(250)<<(header_bytes) << ozendl;
   croust>>(240)<<"Record_bytes ">>(250)<<(record_bytes) << ozendl;
   dbase_record_bytes_total = (fp.membuf.size() - header_bytes);
   fp.reparsa(fielddesc_start);
   while(fp.hasChar()){
      int current_pos = fp.currentPos();
      uint8_t nextchar = fp.parse8("arrayterm");
      if(nextchar == 0x0d){
         break;
      }
      else {
         fp.reparsa(current_pos);
         string fieldname = __chompends(fp.parseBytes(11, "fieldname"));
         uint8_t fieldtype = fp.parse8("fieldtype");
         uint32_t fieldaddress = fp.parse32("fieldaddress"); // not sure what this is
         uint8_t fieldlen = fp.parse8("fieldlen");
         uint8_t fielddeccount = fp.parse8("fielddeccount");
         fp.parseBytes(2, "reserved_for_dbase3plus");
         uint8_t workareaid = fp.parse8("workareaid");
         fp.parseBytes(2, "reserved_for_dbase3plus");
         uint8_t setfieldsflag = fp.parse8("setfieldsflag");
         fp.parseBytes(8, "more reserved stuff");
         if(fp.errored){
            croust>>(196)<<"Errored while parsing field defs" << ozendl;
            return;
         }
         FieldDesc fd;
         fd.name = fieldname;
         fd.type = fieldtype;
         fd.length = fieldlen;
         fd.decimalcount = fielddeccount;
         fielddescs.push_back(fd);
      }
   }
   fp.reparsa(header_bytes);
   int record_count = dbase_record_bytes_total /record_bytes;
   int record_leftover = dbase_record_bytes_total % record_bytes;
   croust>>(196)<<"record count / leftover ">>(250)<<(record_count )>>(220)<<"/">>(250)<<(record_leftover) << ozendl;
   asserts(record_count == records.size(), "")
   { int _rangetotal = record_count;
   for(int i = 0; i < _rangetotal; i++){
      int actual_record_number = i + 1;
      int actual_offset = header_bytes + (i * record_bytes);
      fp.reparsa(actual_offset);
      auto& actual_record = records[i];
      uint8_t delmarker = fp.parse8("delmarker");
      if(delmarker == ' '){
         sloopvec(fielddescs, moo)
            shringer leshringer = fp.parseBytesShringer(moo.length, "parsing field").chompends();
            actual_record.fieldvalues.push_back(leshringer); // yeah, how about that
            moo.regPossibleValue(leshringer); // why not
         sloopend
      }
      else {
      }
   }}
   croust>>(33)<<"Fields and their summaries" << ozendl;
   sloopveccounta(fielddescs, moo, counta)
      croust>>(220)<<(counta )>>(250)<<" : ";
      moo.printSummary();
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::loadShp(const schfile& theschfile){
   static_assert(sizeof(FileSHPHeaderRanges) == 8 * 8);
   shp_fp.init(theschfile);
   croust>>(33)<<"--------- Shapefile Loading ----------" << ozendl;
   croust>>(33)<<"File size : ">>(250)<<(shp_fp.membuf.size()) << ozendl;
   header_filecode = endian_flip32(shp_fp.parse32("can't get filecode"));
   int32_t unused1 = shp_fp.parse32("unused1");
   int32_t unused2 = shp_fp.parse32("unused2");
   int32_t unused3 = shp_fp.parse32("unused3");
   int32_t unused4 = shp_fp.parse32("unused4");
   int32_t unused5 = shp_fp.parse32("unused5");
   header_filelen = endian_flip32(shp_fp.parse32("can't get filelen"));
   header_version = shp_fp.parse32("can't get version");
   header_shapetype_num = shp_fp.parse32("can't get shapetype_num");
   header_shapetype = shapeTypeName(header_shapetype_num);
   FileSHPHeaderRanges * ranges = shp_fp.parseStruct<FileSHPHeaderRanges>();
   if(!ranges){
      shp_fp.err("can't get range headers, wut");
   }
   geo_range.topleft.x = ranges->min_x;
   geo_range.topleft.y = ranges->min_y;
   geo_range.botright.x = ranges->max_x;
   geo_range.botright.y = ranges->max_y;
   headerranges = *ranges;
   croust>>(33)<<"Loaded shp header!" << ozendl;
   croust>>(220)<<"filecode ">>(250)<<(header_filecode) << ozendl;
   croust>>(220)<<"filelength ">>(250)<<(header_filelen) << ozendl;
   croust>>(220)<<"version ">>(250)<<(header_version) << ozendl;
   croust>>(33)<<"Header shapetype ">>(250)<<(header_shapetype) << ozendl;
   int max_recnum = 0;
   while(shp_fp.hasChar()){
      FileSHPRec * rec = shp_fp.parseStruct<FileSHPRec>();
      if(shp_fp.errored){
         return;
      }
      if(!rec){
         break; // done
      }
      int recnum = endian_flip32(rec->record_num);
      int reclen = endian_flip32(rec->record_len);
      max_recnum = max(max_recnum, recnum);
      records.push_back(Record());
      Record& backa = records.back();;
      backa.recnum = recnum;
      uint32_t shapetype_num = shp_fp.parse32("can't get shapetype of a record!");
      if(shp_fp.errored){
         return;
      }
      backa.shapetype_num = shapetype_num;
      backa.shapetype = shapeTypeName(shapetype_num);
      encountered_shapetypes.insert(backa.shapetype);
      if(backa.shapetype == e__(unhandled))
         unknown_types_encountered.insert(backa.shapetype_num);
      int bytes_to_parse = (2 * reclen) - (4);
      asserts(bytes_to_parse >= 0, "")
      shringer lebytes = shp_fp.parseBytesShringer(bytes_to_parse, "failed to get record!");
      if(shp_fp.errored){
         return;
      }
      backa.contentrange = lebytes;
      backa.loadPolygon(*this);
      if(backa.content_errored){
         croust>>(196)<<"Content error @ ">>(220)<<(recnum) << ozendl;
      }
   }
   croust>>(250)<<"Record count ">>(196)<<(records.size() )>>(240)<<" with max recnum ">>(196)<<(max_recnum) << ozendl;
   croust>>(33)<<"encountered shapetypes:" << ozendl;
   sloopvec(encountered_shapetypes, moo)
      croust>>(250)<<"   "<<(moo) << ozendl;
   sloopend
   if(unknown_types_encountered.size() != 0){
      sloopvec(unknown_types_encountered, moo)
         croust>>(196)<<"   unknown shapetype num ">>(250)<<(moo) << ozendl;
      sloopend
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::load(const schfile& theschfile){
   asserts(Morestring::endsWith(theschfile.path(), ".shp"), "file should end with .shp!");
   loadShp(theschfile);
   string dbfpath = Morestring::cutEnd(theschfile.path(), ".shp") + ".dbf";
   loadDbf(dbfpath);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool ShapeFile::errored(){
   if(shp_fp.errored){
      return true;
   }
   if(dbf_fp.errored){
      return true;
   }
   return false;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::printErrors(){
   if(shp_fp.errored){
      shp_fp.printErrors();
   }
   else if(dbf_fp.errored){
      dbf_fp.printErrors();
   }
   else {
      croust>>(220)<<"No errors??" << ozendl;
   }
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::testPrint(){
   sloopveccounta(records, moo, counta)
      croust>>(33)<<"Record # ">>(250)<<(counta) << ozendl;
      oustoust.push(3);
         sloopveccounta(moo.fieldvalues, fieldval, fieldcounta)
            croust>>(250)<<(fieldcounta )>>(220)<<" : ">>(240)<<(fieldval.toString()) << ozendl;
         sloopend
         croust>>(112)<<(moo.points.size() )>>(250)<<" points, ">>(33)<<(moo.parts.size() )>>(250)<<" parts" << ozendl;
      oustoust.pop();
      if(counta > 10){
         break;
      }
   sloopend
   int pointcount = 0;
   int partcount = 0;
   sloopvec(records, moo)
      pointcount += moo.points.size();
      partcount += moo.parts.size();
   sloopend
   croust>>(33)<<"Points Total : ">>(250)<<(pointcount) << ozendl;
   croust>>(112)<<"Parts Total : ">>(250)<<(partcount) << ozendl;
   croust>>(196)<<"Records Total : ">>(250)<<(records.size()) << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::neoCullEdges(float max_perc_diff){
   croust>>(196)<<"culling..." << ozendl;
   sloopveccounta(records, moo, counta)
      if(counta % 100 == 0){
         croust<<"culling... @ "<<(counta) << ozendl;
      }
      moo.performNeoCulling(*this, max_perc_diff);
   sloopend
   croust>>(196)<<"NEO">>(33)<<"Culling finished : ">>(250)<<(original_pointcount )>>(220)<<" => ">>(33)<<(postculled_pointcount) << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::cullPolygonEdges(float dotnum){
   croust>>(240)<<"culling..." << ozendl;
   sloopveccounta(records, moo, counta)
      if(counta % 1000 == 0){
         croust<<"culling... @ "<<(counta) << ozendl;
      }
      moo.cullPolygonEdges(*this, dotnum);
   sloopend
   croust>>(33)<<"Culling finished : ">>(250)<<(original_pointcount )>>(220)<<" => ">>(33)<<(postculled_pointcount) << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::filterContains(int fieldindex, const string& thefilter){
   if(fieldindex >= fielddescs.size()){
      croust>>(196)<<"No fieldindex ">>(250)<<(fieldindex )<<" !" << ozendl;
      return;
   }
   vector<Record> newrecvec;
   sloopvec(records, moo)
      if(Morestring::contains(moo.fieldvalues[fieldindex].toString(), thefilter)){
         newrecvec.push_back(moo);
      }
   sloopend
   records = newrecvec;
   recalculateBounds();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::recalculateBounds(){
   sloopveccounta(records, moo, counta)
      if(counta == 0){
         headerranges.min_x = moo.min_x;
         headerranges.min_y = moo.min_y;
         headerranges.max_x = moo.max_x;
         headerranges.max_y = moo.max_y;
      }
      else {
         headerranges.min_x = min(headerranges.min_x, moo.min_x);
         headerranges.min_y = min(headerranges.min_y, moo.min_y);
         headerranges.max_x = max(headerranges.max_x, moo.max_x);
         headerranges.max_y = max(headerranges.max_y, moo.max_y);
      }
   sloopend
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::filterStarts(int fieldindex, const string& thefilter){
   if(fieldindex >= fielddescs.size()){
      croust>>(196)<<"No fieldindex ">>(250)<<(fieldindex )<<" !" << ozendl;
      return;
   }
   if(fieldindex >= fielddescs.size()){
      croust>>(196)<<"No fieldindex ">>(250)<<(fieldindex )<<" !" << ozendl;
      return;
   }
   vector<Record> newrecvec;
   sloopvec(records, moo)
      if(Morestring::beginsWith(moo.fieldvalues[fieldindex], thefilter)){
         newrecvec.push_back(moo);
      }
   sloopend
   records = newrecvec;
   recalculateBounds();
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::colsToCSV(vector<int> columns){
   string output;
   sloopveccounta(columns, lecol, colnum)
      assert(lecol < fielddescs.size());
      if(colnum != 0)
         output.append(",");
      output.append(fielddescs[lecol].name);
   sloopend
   string exportedcolumns = output;
   output.append("\n");
   sloopvec(records, lerec)
      sloopveccounta(columns, lecol, colnum)
         if(colnum != 0)
            output.append(",");
         assert(lecol < fielddescs.size());
         output.append(Morestring::encaseLiteral(lerec.fieldvalues[lecol].toString()));
      sloopend
      output.append("\n");
   sloopend
   croust<<"outputting columns to columns.csv : ";
   croust>>(220)<<(exportedcolumns) << ozendl;
   schfile("columns.csv") <<= output;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::determineFillRate(){
   map<int, int> decile_counts;
   map<int, pair<int, int> > area_percentile_count_triangles;
   map<int, int> tringcounts;
   float block_area_max = 0;
   float last_area_max = 0.0908096;
   sloopvec(records, moo)
      using Epoint = std::array<float, 2>;
      vector<vector<Epoint>> tespoints; // for earcut
      tespoints.push_back(vector<Epoint>()); 
      sloopvec(moo.points, lepoint)
         tespoints.back().push_back({lepoint.x, lepoint.y});
      sloopend
      vector<int> poly_indices = mapbox::earcut<int>(tespoints);
      float block_area = (moo.max_x - moo.min_x) * (moo.max_y - moo.min_y);
      int area_perc = 100 * block_area / last_area_max;
      auto& area_lepair = area_percentile_count_triangles[area_perc];
      area_lepair.first++;
      block_area_max = max(block_area, block_area_max);
      float area = 0;
      int tringcount = poly_indices.size() / 3;
      area_lepair.second += tringcount;
      { int _rangetotal = tringcount;
      for(int i = 0; i < _rangetotal; i++){
         int i1 = i * 3;
         int i2 = i * 3 + 1;
         int i3 = i * 3 + 2;
         auto p1 = moo.points[poly_indices[i1]];
         auto p2 = moo.points[poly_indices[i2]];
         auto p3 = moo.points[poly_indices[i3]];
         area += Point2D::areaOfTriangle(Point2D(p1.x, p1.y), Point2D(p2.x, p2.y), Point2D(p3.x, p3.y));
      }}
      tringcounts[tringcount]++;
      float ratio = area / block_area;
      int perc = ratio * 10;
      moo.fillrate = ratio;
      decile_counts[perc]++;
   sloopend
   croust>>(250)<<"Area max : ">>(220)<<(block_area_max) << ozendl;
   croust>>(33)<<"Decile fillrates" << ozendl;
   sloopmapex(decile_counts, eh, moo)
      croust>>(250)<<(eh )>>(220)<<" : ">>(240)<<(moo) << ozendl;
   sloopend
   int tring_total = 0;
   croust>>(250)<<"Areapercs" << ozendl;
   sloopmapex(area_percentile_count_triangles, eh, moo)
      croust>>(250)<<(eh )>>(196)<<" : ">>(240)<<(moo.first )>>(220)<<" blocks / ">>(240)<<(moo.second) << ozendl;
   sloopend
   croust>>(250)<<"Tringcounts" << ozendl;
   sloopmapex(tringcounts, eh, moo)
      croust>>(250)<<(eh )>>(196)<<" : ">>(240)<<(moo )>>(220)<<" => ">>(240)<<(eh * moo) << ozendl;
      tring_total += eh * moo;
   sloopend
   croust>>(250)<<"Tring total : ">>(220)<<(tring_total) << ozendl;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ShapeFile::saveTo(ProcessedShapeFile& outfile){
   outfile.ranges.topleft = Point2D(headerranges.min_x, headerranges.min_y);
   outfile.ranges.botright = Point2D(headerranges.max_x, headerranges.max_y);
   sloopvec(records, moo)
      outfile.records.push_back(ProcessedRecord());
      auto& newa = outfile.records.back();
      newa.fillrate = moo.fillrate;
      newa.points = moo.points;
      using Epoint = std::array<float, 2>;
      vector<vector<Epoint>> tespoints; // for earcut
      tespoints.push_back(vector<Epoint>()); 
      sloopvec(moo.points, lepoint)
         tespoints.back().push_back({lepoint.x, lepoint.y});
      sloopend
      newa.poly_indices = mapbox::earcut<int>(tespoints);
      newa.range.topleft = Point2D(moo.min_x, moo.min_y);
      newa.range.botright = Point2D(moo.max_x, moo.max_y);
      sloopvec(moo.fieldvalues, fv)
         newa.attribs.push_back(fv.toString());
      sloopend
   sloopend
}
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
float determineArea(vector<Point2D>& vecca){
   using Epoint = std::array<float, 2>;
   vector<vector<Epoint>> tespoints; // for earcut
   tespoints.push_back(vector<Epoint>()); 
   sloopvec(vecca, lepoint)
      tespoints.back().push_back({lepoint.x, lepoint.y});
   sloopend
   vector<int> poly_indices = mapbox::earcut<int>(tespoints);
   float area = 0;
   int tringcount = poly_indices.size() / 3;
   { int _rangetotal = tringcount;
   for(int i = 0; i < _rangetotal; i++){
      int i1 = i * 3;
      int i2 = i * 3 + 1;
      int i3 = i * 3 + 2;
      auto p1 = vecca[poly_indices[i1]];
      auto p2 = vecca[poly_indices[i2]];
      auto p3 = vecca[poly_indices[i3]];
      area += Point2D::areaOfTriangle(Point2D(p1.x, p1.y), Point2D(p2.x, p2.y), Point2D(p3.x, p3.y));
   }}
   return area;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
