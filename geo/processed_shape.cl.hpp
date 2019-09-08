////////////////////////////////////////////////////////////
#pragma once
namespace std{}; using namespace std;
////////////////////////////////////////////////////////////
#include "cloth_classdef.cl.hpp"
#include "../shing/moremath.hpp"
#include "../shing/flectordefs.hpp"
#include "../shing/classdef.hpp"
#include "../shing/flector.hpp"
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class ProcessedRecord{
   public:
   //-------------------------------------------------------
   vector<string> attribs;
   vector<Point2D> points;
   vector<int> poly_indices;
   FloatRect2D range;
   float fillrate = 0;
   //-------------------------------------------------------
   flector_subbie_decl;
   ProcessedRecord();
   void initflec();
};
////////////////////////////////////////////////////////////
class ProcessedShapeFile{
   public:
   //-------------------------------------------------------
   vector<ProcessedRecord> records;
   FloatRect2D ranges;
   //-------------------------------------------------------
   flector_subbie_decl;
   ProcessedShapeFile();
   void initflec();
   void loadFile(const schfile& leschfile);
   void saveFile(const schfile& leschfile);
   Point2D ztoise(float x, float y);
   Point2D ztoise(const Point2D& lepos);
   FloatRect2D ztoise(const FloatRect2D& lerect);
   //-------------------------------------------------------
};
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
