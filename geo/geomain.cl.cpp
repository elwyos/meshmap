////////////////////////////////////////////////////////////
#include "shp.cl.hpp"
#include "../shing/consolearg.hpp"
#include "processed_shape.cl.hpp"
#include "meshblocks.cl.hpp"
#include "geomain.cl.hpp"
////////////////////////////////////////////////////////////
int main(int argc, char * argv[] );
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
int main(int argc, char * argv[] ){
   CargParser parsa(argc, argv);
   if((parsa-- << "mb")){
      MBHost mbhost;
      mbhost.loadRaw();
      croust>>(33)<<"Saved to ">>(220)<<"meshblocks.flec" << ozendl;
      mbhost.saveFile("meshblocks.flec");
      return 0;
   }
   string filename = parsa--.parseFileName();
   ShapeFile shpfile;
   shpfile.load(filename);
   if(shpfile.errored()){
      shpfile.printErrors();
   }
   else {
      shpfile.testPrint();
   }
   string outfilename = "temp.geo";
   bool columns_exported = false;
   while(parsa--.hasChar()){
      if(parsa-- << "-o"){
         outfilename = parsa--.parseFileName();
         croust>>(196)<<"outfile ">>(250)<<(outfilename) << ozendl;
      }
      else if(parsa-- << "-filterstart"){
         int fieldindex = parsa--.parseNumber();
         string contents = parsa--.parseLabel();
         shpfile.filterStarts(fieldindex, contents);
      }
      else if(parsa-- << "-filtercontains"){
         int fieldindex = parsa--.parseNumber();
         string contents = parsa--.parseLabel();
         shpfile.filterContains(fieldindex, contents);
      }
      else if(parsa-- << "-fillrate"){
         shpfile.determineFillRate();
      }
      else if(parsa-- << "-culledges"){
         float dotnum = parsa--.parseFloat();
         shpfile.cullPolygonEdges(dotnum);
      }
      else if(parsa-- << "-neocull"){
         float dotnum = parsa--.parseFloat();
         shpfile.neoCullEdges(dotnum);
      }
      else if(parsa-- << "-exportcols"){
         vector<int> columns;
         while(parsa--.peekingAtNumbers()){
            columns.push_back(parsa--.parseNumber());
         }
         columns_exported = true;
         shpfile.colsToCSV(columns);
      }
      else {
         parsa.error("unknown arg mode");
      }
   }
   if(columns_exported){
      croust>>(196)<<"Already exported columns. done." << ozendl;
      return 0;
   }
   ProcessedShapeFile psf;
   croust<<"exporting to.. "<<(outfilename) << ozendl;
   shpfile.saveTo(psf);
   croust<<"saving... " << ozendl;
   psf.saveFile(outfilename); // test
   croust<<"done!" << ozendl;
   return 0;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
