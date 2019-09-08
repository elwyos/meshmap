//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#bodinc shp consolearg processed_shape meshblocks
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
[nonamespace]
--/main(int argc, char * argv[]) -> int
   
   CargParser parsa(argc, argv);
   
   \if(parsa-- << "mb")
      
      // finish here
      MBHost mbhost;
      mbhost.loadRaw();
      \\/ [33] "Saved to " [220] "meshblocks.flec"
      mbhost.saveFile("meshblocks.flec");
      
      return 0;
   
   string filename = parsa--.parseFileName();
   
   ShapeFile shpfile;
   shpfile.load(filename);
   
   \if shpfile.errored()
      shpfile.printErrors();
   \else
      shpfile.testPrint();
   
   //shpfile.cullPolygonEdges();
   // wait for a bit
   
   
   string outfilename = "temp.geo";
   
   bool columns_exported = false;
   
   \while parsa--.hasChar()
      \if parsa-- << "-o"
         outfilename = parsa--.parseFileName();
         \\/[196] "outfile " [250] outfilename
      \elif parsa-- << "-filterstart"
         int fieldindex = parsa--.parseNumber();
         string contents = parsa--.parseLabel();
         
         shpfile.filterStarts(fieldindex, contents);
         
      \elif parsa-- << "-filtercontains"
         int fieldindex = parsa--.parseNumber();
         string contents = parsa--.parseLabel();
         
         shpfile.filterContains(fieldindex, contents);
      \elif parsa-- << "-fillrate"
         shpfile.determineFillRate();
      \elif parsa-- << "-culledges"
         float dotnum = parsa--.parseFloat();
         shpfile.cullPolygonEdges(dotnum);
      \elif parsa-- << "-neocull"
         float dotnum = parsa--.parseFloat();
         shpfile.neoCullEdges(dotnum);
      \elif parsa-- << "-exportcols"
         
         vector<int> columns;
         
         \while parsa--.peekingAtNumbers()
            columns.push_back(parsa--.parseNumber());
            
         columns_exported = true;
         shpfile.colsToCSV(columns);
         
      \else
         parsa.error("unknown arg mode");
   
   \if columns_exported
      \\/[196] "Already exported columns. done."
      return 0;
   
   // and see if it printed
   ProcessedShapeFile psf;
   \\/ "exporting to.. " outfilename
   shpfile.saveTo(psf);
   \\/ "saving... "
   psf.saveFile(outfilename); // test
   \\/ "done!"
   
   return 0;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
