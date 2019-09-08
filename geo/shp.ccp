//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc endian morestring processed_shape neocontext curvemapper
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc <algorithm>
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc "../include/earcut.hpp"
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
FieldDesc/regPossibleValue(const shringer& lestr)

   possible_hashes.insert(Morestring::hash3(lestr));

   \if possible_values_collection.size() >= 20
      return;
   
   possible_values_collection.insert(__chompends(lestr.toString()));
//////////////////////////////////////////////////////////////////////////////////////////
FieldDesc/printSummary()
   \\/[33] name " " [250] ((char)type) [240] " - " length " bytes"
   
   \if possible_values_collection.size() != 0
      
      oustoust[33] << "possible_hashes : " >> 250 << possible_hashes.size() << ozendl;
      
      oustoust.push(3);
         
         \sloopset possible_values_collection
            \if moo == ""
               \\/ [196] "\"\"" [100] ", " \
            \else
               \\/ [244] moo [100]", " \
   
         \\/
      
      oustoust.pop();
   \else
      
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Record/loadPolygon(ShapeFile& master)
   
   \if shapetype != `\polygon
      return;
   
   // 32 bytes, so 4 doubles
   EasyParser parsa(contentrange);
   \if !parsa.absorbDouble(min_x)
      content_errored = true; return;
   \if !parsa.absorbDouble(min_y)
      content_errored = true; return;
   \if !parsa.absorbDouble(max_x)
      content_errored = true; return;
   \if !parsa.absorbDouble(max_y)
      content_errored = true; return;
      
   zto_ranges.topleft = master.ztoise(min_x, min_y);
   zto_ranges.botright = master.ztoise(max_x, max_y);
   
   uint32_t partcount = 0;
   uint32_t pointcount = 0;
   
   \if !parsa.absorbInt(partcount)
      content_errored = true; return;
   \if !parsa.absorbInt(pointcount)
      content_errored = true; return;
      
   
   \looprange i [0..partcount)
      uint32_t somepart = 0;
      \if !parsa.absorbInt(somepart)
         content_errored = true; return;
      parts.push_back(somepart);
   
   \looprange i [0..pointcount)
      double pointx = 0;
      double pointy = 0;
      
      \if !parsa.absorbDouble(pointx)
         content_errored = true; return;
      \if !parsa.absorbDouble(pointy)
         content_errored = true; return;
         
      \if i == 0
         min_x = max_x = pointx;
         min_y = max_y = pointy;
      \else
         min_x = min(min_x, pointx);
         min_y = min(min_y, pointy);
         
         max_x = max(max_x, pointx);
         max_y = max(max_y, pointy);
         
      points.push_back(Point2D(pointx, pointy));

   // cool, I guess
   

   
   // let's try reduce polygons, for now
   \sloop points
      ztopoints.push_back(master.ztoise(moo.x, moo.y));

//////////////////////////////////////////////////////////////////////////////////////////
--/determineArea(vector<Point2D>& vecca) -> float
   using Epoint = std::array<float, 2>;
   vector<vector<Epoint>> tespoints; // for earcut
   tespoints.push_back(vector<Epoint>()); 
   
   sloopvec(vecca, lepoint)
      tespoints.back().push_back({lepoint.x, lepoint.y});
   sloopend
   
   vector<int> poly_indices = mapbox::earcut<int>(tespoints);
   
   float area = 0;
   
   int tringcount = poly_indices.size() / 3;
   
   \looprange i [0..tringcount)
      int i1 = i * 3;
      int i2 = i * 3 + 1;
      int i3 = i * 3 + 2;
      auto p1 = vecca[poly_indices[i1]];
      auto p2 = vecca[poly_indices[i2]];
      auto p3 = vecca[poly_indices[i3]];
      
      area += Point2D::areaOfTriangle(Point2D(p1.x, p1.y), Point2D(p2.x, p2.y), Point2D(p3.x, p3.y));
      
   return area;
//////////////////////////////////////////////////////////////////////////////////////////
Record/massCullStep(float max_percent_diff) -> bool
   
   \if points.size() <= max_reducted_size
      return false; // do these ones with neo
   
   // decimate it
   
   static Shing::Random srando;
   
   float best_diff = max_percent_diff;
   vector<Point2D> cutcand;
   
   \looprange phase [0..30)
      
      vector<int> indices;
      \looprange i [0..points.size())
         indices.push_back(i);
         
      srando.shuffle(indices);
      
      int toletlive = 95 * indices.size() / 100;
      indices.resize(toletlive);
      
      sort(indices.begin(), indices.end());
      
      vector<Point2D> newpoints;
      \sloop indices, leindex
         newpoints.push_back(points[leindex]);
      
      float newarea = determineArea(newpoints);
      float fac = 100 * (newarea / original_area);
      float difference = fabs(fac - 100);
      
      \if difference < best_diff
         best_diff = difference;
         cutcand = newpoints;
   
   \if cutcand.size() != 0
      points = cutcand;
      return true;
   
   return false;
         
         
      
   
   
//////////////////////////////////////////////////////////////////////////////////////////
Record/neoCullStep(float max_percent_diff) -> bool
   
   \if points.size() <= max_reducted_size
      return false;
   
   // there are n possibles
   int pointcount = points.size();
   
   int best_index = -1;
   float best_cull_change = max_percent_diff; // 5 is max
   
   \looprange i [0..pointcount)
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
      \if difference < best_cull_change
         best_index = i;
         best_cull_change = difference;
      
   \if best_index != -1
      points.erase(points.begin() + best_index); // yeah, boy
      return true;
   
   return false;
   
//////////////////////////////////////////////////////////////////////////////////////////
Record/performNeoCulling(ShapeFile& master, float max_percent_diff)
   
   master.original_pointcount += points.size();
   
   original_area = determineArea(points);
   
   definecurve(area_minner, (500, 40)
                            (100, 10)
                            (50, 4));
                            
   max_reducted_size = area_minner.get(points.size());
   
   bool printit = points.size() > 300;
   
   \if printit
      cout << "Performing neoculling... : " << points.size() << endl;
   
   splaicht(neoculling);
   cullPolygonEdges(master, 0.76f); // let's try this
   
   \if printit
      cout << "New points :" << points.size() << endl;
   
   \while true
      bool succ = massCullStep(max_percent_diff);
      \if !succ
         break;
   
   \if printit
      cout << "post masscull : " << points.size() << endl;
   
   \while true
      bool succ = neoCullStep(max_percent_diff);
      \if !succ
         break;
      
      // otherwise, continue, I guess
      
   \if printit
      cout << "Postcull size : " << points.size() << endl;
      cout << "------------" << endl;
      
   master.postculled_pointcount += points.size();
   
//////////////////////////////////////////////////////////////////////////////////////////
Record/cullPolygonEdges(ShapeFile& master, float dotnum)
   
   gplaicht(neoculling);
   if(!neoculling)
      master.original_pointcount += points.size();
   
   vector<Point2D> postculled = points;
   
   int curindex = 0;
   \while curindex + 2 < postculled.size()
      Point2D starting_diff = (postculled[curindex + 1] - postculled[curindex]).normalise();
      
      \while curindex + 2 < postculled.size()
         Point2D next_diff = (postculled[curindex + 2] - postculled[curindex + 1]).normalise();
         
         float thedot = dot(next_diff, starting_diff);
         if(thedot > dotnum){
            // can cull
            postculled.erase(postculled.begin() + (curindex + 1)); // remove the corner
         } else {
            curindex++;
            break; // get to the otuer loop
         }
         
   points = postculled;
   
   if(!neoculling)
      master.postculled_pointcount += points.size();
   
   ztopoints.clear();
   \sloop points
      ztopoints.push_back(master.ztoise(moo.x, moo.y));

   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/ShapeFile()

//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/ztoise(double x, double y) -> Point2D
   Point2D ret;
   ret.x = (x - headerranges.min_x) / (headerranges.max_x - headerranges.min_x);
   ret.y = (y - headerranges.min_y) / (headerranges.max_y - headerranges.min_y);
   return ret;
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/shapeTypeName(int thenum) -> eostri
   \switch thenum
      \case 0
         return `\nullshape;
      \case 1
         return `\point;
      \case 3
         return `\polyline;
      \case 5
         return `\polygon;
      \case 8
         return `\multipoint;
      \case 11
         return `\pointz;
      \case 13
         return `\polylinez;
      \case 15
         return `\polygonz;
      \default
         return `\unhandled;
      
   return `\whut;
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/syncDbaseVersionName()
   \switch dbase_version_num
      \case 0x02
         dbase_version_name = "FoxBase 1.0";
      \case 0x03
         dbase_version_name = "FoxBase 2.x / dBASE III";
      \case 0x83
         dbase_version_name = "FoxBase 2.x / dBASE III with memo file";
      \case 0x30
         dbase_version_name = "Visual FoxPro";
      \case 0x31
         dbase_version_name = "Visual FoxPro with auto increment";
      \case 0x32
         dbase_version_name = "Visual FoxPro with varchar/varbinary";
      \case 0x43
         dbase_version_name = "dBASE IV SQL Table, no memo file";
      \case 0x63
         dbase_version_name = "dBASE IV SQL System, no memo file";
      \case 0x8b
         dbase_version_name = "dBASE IV with memo file";
      \case 0xcb
         dbase_version_name = "dBASE IV SQL Table with memo file";
      \case 0xfb
         dbase_version_name = "FoxPro 2";
      \case 0xf5
         dbase_version_name = "FoxPro 2 with memo file";
      \default
         dbase_version_name = "unknown version name";
      
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/loadDbf(const schfile& theschfile)
   
   auto& fp = dbf_fp;
   
   fp.init(theschfile);
   
   \\/[220] "--------- DBF Loading ----------"
   \\/[220] "file size " [250] fp.membuf.size()
   
   \if fp.errored
      return;
   
   uint8_t versionbyte = fp.parse8("versionbyte");
   dbase_version_num = versionbyte;
   syncDbaseVersionName();
   
   \\/ [33] "dbase version name : " [250] dbase_version_name
   
   string lastupdateformat = fp.parseBytes(3, "lastupdateformat");
   uint32_t rec_count = fp.parse32("rec_count");
   
   uint16_t header_bytes = fp.parse16("header_bytes");
   uint16_t record_bytes = fp.parse16("record_bytes");
   
   fp.parseBytes(3, "reserved");
   fp.parseBytes(13, "more reserved");
   fp.parseBytes(4, "even more reserved");
   
   int fielddesc_start = fp.currentPos();
   
   
   \\/ [240] "Header_bytes " [250] header_bytes
   \\/ [240] "Record_bytes " [250] record_bytes
   
   dbase_record_bytes_total = (fp.membuf.size() - header_bytes);
   
   fp.reparsa(fielddesc_start);
   
   \while fp.hasChar()
      
      int current_pos = fp.currentPos();
      
      uint8_t nextchar = fp.parse8("arrayterm");
      \if nextchar == 0x0d
         // cool, break
         break;
      \else
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
         
         \if fp.errored
            \\/[196] "Errored while parsing field defs"
            return;
         
         FieldDesc fd;
         fd.name = fieldname;
         fd.type = fieldtype;
         fd.length = fieldlen;
         fd.decimalcount = fielddeccount;
         fielddescs.push_back(fd);
   
   
   fp.reparsa(header_bytes);
   
   int record_count = dbase_record_bytes_total /record_bytes;
   int record_leftover = dbase_record_bytes_total % record_bytes;
   \\/ [196] "record count / leftover " [250]record_count [220] "/" [250] record_leftover
   
   \assert record_count == records.size()
   // since we're doing shapefile anyways..
   
   \looprange i [0..record_count)
   
      int actual_record_number = i + 1;
      
      int actual_offset = header_bytes + (i * record_bytes);
      fp.reparsa(actual_offset);
      
      
      auto& actual_record = records[i];
      
      uint8_t delmarker = fp.parse8("delmarker");
      \if delmarker == ' '
         \sloopvec fielddescs
            shringer leshringer = fp.parseBytesShringer(moo.length, "parsing field").chompends();
            actual_record.fieldvalues.push_back(leshringer); // yeah, how about that
            moo.regPossibleValue(leshringer); // why not
            // they should be all strings
      \else
         // record is deleted, apparently
   
   
   \\/[33] "Fields and their summaries"
   \sloopvec fielddescs, moo, counta
      \\/ [220] counta [250] " : " \
      moo.printSummary();
   
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/loadShp(const schfile& theschfile)
   static_assert(sizeof(FileSHPHeaderRanges) == 8 * 8);
   
   // let's see eh
   shp_fp.init(theschfile);
   
   \\/[33] "--------- Shapefile Loading ----------"
   
   \\/[33] "File size : " [250] shp_fp.membuf.size()
   
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
   \if !ranges
      shp_fp.err("can't get range headers, wut");
      
   geo_range.topleft.x = ranges->min_x;
   geo_range.topleft.y = ranges->min_y;
   geo_range.botright.x = ranges->max_x;
   geo_range.botright.y = ranges->max_y;
      
   headerranges = *ranges;
      
   \\/ [33] "Loaded shp header!"
   \\/ [220] "filecode " [250] header_filecode
   \\/ [220] "filelength " [250] header_filelen
   \\/ [220] "version " [250] header_version

   \\/[33] "Header shapetype " [250] header_shapetype
   
   int max_recnum = 0;
   
   // cool
   \while shp_fp.hasChar()
      FileSHPRec * rec = shp_fp.parseStruct<FileSHPRec>();
      
      if(shp_fp.errored){
         return;
      }
      
      \if !rec
         break; // done
      
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
      if(backa.shapetype == `\unhandled)
         unknown_types_encountered.insert(backa.shapetype_num);
         
         
      int bytes_to_parse = (2 * reclen) - (4);
      \assert bytes_to_parse >= 0;
      
      shringer lebytes = shp_fp.parseBytesShringer(bytes_to_parse, "failed to get record!");
      if(shp_fp.errored){
         return;
      }
      backa.contentrange = lebytes;
      backa.loadPolygon(*this);
      
      \if backa.content_errored
         \\/[196] "Content error @ " [220] recnum
   
   
   \\/ [250] "Record count " [196] records.size() [240] " with max recnum " [196] max_recnum
   
   \\/ [33] "encountered shapetypes:"
   \sloop encountered_shapetypes
      \\/ [250] "   " moo
   
   \if unknown_types_encountered.size() != 0
      \sloop unknown_types_encountered
         \\/ [196] "   unknown shapetype num " [250] moo
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/load(const schfile& theschfile)
   
   asserts(Morestring::endsWith(theschfile.path(), ".shp"), "file should end with .shp!");
   loadShp(theschfile);
   
   string dbfpath = Morestring::cutEnd(theschfile.path(), ".shp") + ".dbf";
   loadDbf(dbfpath);
   
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/errored() -> bool
   \if shp_fp.errored
      return true;
   \if dbf_fp.errored
      return true;
   return false;
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/printErrors()
   \if shp_fp.errored
      shp_fp.printErrors();
   \elif dbf_fp.errored
      dbf_fp.printErrors();
   \else
      \\/[220] "No errors??"
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/testPrint()

   \sloop records, moo, counta
      
      \\/[33] "Record # " [250] counta
      oustoust.push(3);
         \sloop moo.fieldvalues, fieldval, fieldcounta
            \\/[250] fieldcounta [220] " : " [240] fieldval.toString()
         
         \\/ [112] moo.points.size() [250] " points, " [33] moo.parts.size() [250] " parts"
      oustoust.pop();
      
      
      
      \if counta > 10
         break;
         
   int pointcount = 0;
   int partcount = 0;
   
   \sloop records
      pointcount += moo.points.size();
      partcount += moo.parts.size();
      
   \\/[33] "Points Total : " [250] pointcount
   \\/[112] "Parts Total : " [250] partcount
   \\/[196] "Records Total : " [250] records.size()
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/neoCullEdges(float max_perc_diff)
   \\/ [196] "culling..."

   \sloop records, moo, counta
      \if counta % 100 == 0
         \\/ "culling... @ " counta
      moo.performNeoCulling(*this, max_perc_diff);
   
   \\/ [196] "NEO" [33] "Culling finished : " [250] original_pointcount [220] " => " [33] postculled_pointcount
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/cullPolygonEdges(float dotnum)

   \\/ [240] "culling..."

   \sloop records, moo, counta
      \if counta % 1000 == 0
         \\/ "culling... @ " counta
      moo.cullPolygonEdges(*this, dotnum);
   
   \\/ [33] "Culling finished : " [250] original_pointcount [220] " => " [33] postculled_pointcount
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/filterContains(int fieldindex, const string& thefilter)
   
   \if fieldindex >= fielddescs.size()
      \\/[196] "No fieldindex " [250] fieldindex " !"
      return;
   
   vector<Record> newrecvec;
   \sloop records
      \if Morestring::contains(moo.fieldvalues[fieldindex].toString(), thefilter)
         newrecvec.push_back(moo);
   
   records = newrecvec;
   recalculateBounds();
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/recalculateBounds()
   
   \sloop records, moo, counta
      
      \if counta == 0
      
         headerranges.min_x = moo.min_x;
         headerranges.min_y = moo.min_y;
         
         headerranges.max_x = moo.max_x;
         headerranges.max_y = moo.max_y;
      
      \else
         headerranges.min_x = min(headerranges.min_x, moo.min_x);
         headerranges.min_y = min(headerranges.min_y, moo.min_y);
         
         headerranges.max_x = max(headerranges.max_x, moo.max_x);
         headerranges.max_y = max(headerranges.max_y, moo.max_y);
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/filterStarts(int fieldindex, const string& thefilter)
   
   \if fieldindex >= fielddescs.size()
      \\/[196] "No fieldindex " [250] fieldindex " !"
      return;
      
   \if fieldindex >= fielddescs.size()
      \\/[196] "No fieldindex " [250] fieldindex " !"
      return;
   
   vector<Record> newrecvec;
   \sloop records
      \if Morestring::beginsWith(moo.fieldvalues[fieldindex], thefilter)
         newrecvec.push_back(moo);
   
   records = newrecvec;
   recalculateBounds();
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/colsToCSV(vector<int> columns)
   
   string output;
   
   \sloop columns, lecol, colnum
      assert(lecol < fielddescs.size());
      
      if(colnum != 0)
         output.append(",");
      
      output.append(fielddescs[lecol].name);
      
   string exportedcolumns = output;
   
   output.append("\n");
   
   \sloop records, lerec
      \sloop columns, lecol, colnum
         if(colnum != 0)
            output.append(",");
         
         assert(lecol < fielddescs.size());
         output.append(Morestring::encaseLiteral(lerec.fieldvalues[lecol].toString()));
         
      output.append("\n");
   
   \\/ "outputting columns to columns.csv : " \
   \\/ [220] exportedcolumns
   schfile("columns.csv") <<= output;
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/determineFillRate()
   
   map<int, int> decile_counts;
   map<int, pair<int, int> > area_percentile_count_triangles;
   
   map<int, int> tringcounts;
   
   float block_area_max = 0;
   
   float last_area_max = 0.0908096;
   
   
   \sloop records
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
      \looprange i [0..tringcount)
         int i1 = i * 3;
         int i2 = i * 3 + 1;
         int i3 = i * 3 + 2;
         auto p1 = moo.points[poly_indices[i1]];
         auto p2 = moo.points[poly_indices[i2]];
         auto p3 = moo.points[poly_indices[i3]];
         
         area += Point2D::areaOfTriangle(Point2D(p1.x, p1.y), Point2D(p2.x, p2.y), Point2D(p3.x, p3.y));
      
      tringcounts[tringcount]++;
      
      float ratio = area / block_area;
      int perc = ratio * 10;
      
      moo.fillrate = ratio;
      
      decile_counts[perc]++;
   
   \\/[250] "Area max : " [220] block_area_max
   
   \\/[33] "Decile fillrates"
   \sloopmap decile_counts
      \\/[250] eh [220] " : " [240] moo
   
   int tring_total = 0;
   
   \\/[250] "Areapercs"
   \sloopmap area_percentile_count_triangles
      \\/[250] eh [196] " : " [240] moo.first [220] " blocks / " [240] moo.second
   
   \\/[250] "Tringcounts"
   \sloopmap tringcounts
      \\/[250] eh [196] " : " [240] moo [220] " => " [240] (eh * moo)
      tring_total += eh * moo;
   
   \\/[250] "Tring total : " [220] tring_total
   
   
//////////////////////////////////////////////////////////////////////////////////////////
ShapeFile/saveTo(ProcessedShapeFile& outfile)
   
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
      
      // cool, I guess
      
   sloopend
   
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
