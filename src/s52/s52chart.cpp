#include "s52chart.h"

#include <QDebug>
#include <QList>

#include <algorithm>

#include "../common/triangulate.h"
#include "../common/rlimath.h"

#include "s57condsymb.h"

using namespace RLIMath;
using namespace S52;

#define EQUAL_EPS 0.00000001

Chart::Chart(char* file_name, S52References* ref) {
  isOk = false;
  _ref = ref;
  sndg_layer = nullptr;
  //qDebug() << file_name;

  // Open OGR data source
  RegisterOGRS57();
  OGRDataSource* poDS = OGRSFDriverRegistrar::Open( file_name, FALSE, nullptr );

  // Failed to open chart
  if (poDS == nullptr)
    return;

  // DEPCNT max VALDCO
  OGRLayer* poLayer = poDS->GetLayerByName("DEPCNT");
  OGRFeature* feat = nullptr;
  QVector<double> valdcoVec;
  poLayer->ResetReading();
  while ((feat = poLayer->GetNextFeature()) != nullptr)
    if (feat->GetFieldIndex("VALDCO") >= 0)
      valdcoVec << feat->GetFieldAsDouble("VALDCO");
  std::sort(valdcoVec.begin(), valdcoVec.end());
  for (auto v: valdcoVec)
    if (v > CONST_SAFETY_DEPTH) {
      _m_next_safe_cnt = v;
      break;
    }

  // iterate through chart layers
  for( int i = 0; i < poDS->GetLayerCount(); i++ ) {
    poLayer = poDS->GetLayer(i);
    QString layer_name = poLayer->GetName();

    //qDebug() << "Reading layer #" << i << layer_name;

    //QRectF fRect(QPointF(144.1, 13.7), QSizeF(20.0, 20.0));
    //poLayer->SetSpatialFilterRect(fRect.left(), fRect.top(), fRect.right(), fRect.bottom());
    OGRGeometry* spatFilter = nullptr; //poLayer->GetSpatialFilter();

    if (layer_name == "M_COVR") {
      OGREnvelope oExt;
      if (!(poLayer->GetExtent(&oExt, TRUE) == OGRERR_NONE))
        return;

      min_lat = static_cast<float>(oExt.MinY);
      max_lat = static_cast<float>(oExt.MaxY);
      min_lon = static_cast<float>(oExt.MinX);
      max_lon = static_cast<float>(oExt.MaxX);

      //qDebug() << min_lat << "-" << max_lat;
      //qDebug() << min_lon << "-" << max_lon;
    }

    poLayer->ResetReading();
    if (layer_name == "SOUNDG") {
      if (!readSoundingLayer(poLayer, spatFilter))
        return;

      continue;
    }

    if (layer_name == "LNDARE"
     || layer_name == "CANALS"
     || layer_name == "LAKARE"
     || layer_name == "LNDRGN"
     || layer_name == "SEAARE") {
      readTextLayer(poLayer);
    }

    poLayer->ResetReading();
    if (!readLayer(poLayer, ref, poDS)) {
      qDebug() << "Failed reading layer " + layer_name;
      return;
    }
  }

  isOk = true;
}


Chart::~Chart() {
  clear();
}

bool Chart::readTextLayer(OGRLayer* poLayer) {
  OGRFeature* poFeature = nullptr;
  QString name;
  OGRPoint centroid;

  TextLayer* layer = new TextLayer();

  while( (poFeature = poLayer->GetNextFeature()) != nullptr ) {
    QString name = poFeature->GetFieldAsString("OBJNAM");

    if (name.trimmed().length() > 0) {
      poFeature->StealGeometry()->Centroid(&centroid);
      layer->points.push_back(static_cast<float>(centroid.getY()));
      layer->points.push_back(static_cast<float>(centroid.getX()));
      layer->texts.push_back(name.trimmed());
    }

    OGRFeature::DestroyFeature( poFeature );
  }

  text_layers.insert(QString(poLayer->GetName()), layer);

  return true;
}


bool Chart::readSoundingLayer(OGRLayer* poLayer, const OGRGeometry* spatFilter) {
  OGRFeature* poFeature = nullptr;

  if (sndg_layer != nullptr)
    return false;

  sndg_layer = new SndgLayer();

  while( (poFeature = poLayer->GetNextFeature()) != nullptr ) {
    for (int i = 0; i < poFeature->GetGeomFieldCount(); i++) {
      OGRGeometry* geom = poFeature->GetGeomFieldRef(i);
      OGRwkbGeometryType geom_type = geom->getGeometryType();

      if (geom_type == wkbMultiPoint25D) {
        OGRMultiPoint* mp = static_cast<OGRMultiPoint*>(geom);

        for (int j = 0; j < mp->getNumGeometries(); j++) {
          OGRPoint* p = static_cast<OGRPoint*>(mp->getGeometryRef(j));

          if (spatFilter == nullptr || spatFilter->Contains(p)) {
            sndg_layer->points.push_back(static_cast<float>(p->getY()));
            sndg_layer->points.push_back(static_cast<float>(p->getX()));
            sndg_layer->depths.push_back(p->getZ());
          }
        }
      }
    }

    OGRFeature::DestroyFeature( poFeature );
  }

  return true;
}

void Chart::clear() {
  for (int i = 0; i < area_layers.keys().size(); i++)
    delete area_layers[area_layers.keys()[i]];

  for (int i = 0; i < line_layers.keys().size(); i++)
    delete line_layers[line_layers.keys()[i]];

  for (int i = 0; i < mark_layers.keys().size(); i++)
    delete mark_layers[mark_layers.keys()[i]];

  delete sndg_layer;
}


QMap<QString, QVariant> Chart::getOGRFeatureAttributes(OGRFeature* obj, const QMap<QString, std::pair<int, OGRFieldType>>& fields) {
  QMap<QString, QVariant> featAttrs;

  int fldListCount;
  QList<QVariant> lst;

  for(auto fName: fields.keys()) {
    auto fldDef = fields[fName];
    switch (fldDef.second) {
      case OFTInteger:
        featAttrs.insert(fName, obj->GetFieldAsInteger(fldDef.first));
        break;
      case OFTString:
        featAttrs.insert(fName, obj->GetFieldAsString(fldDef.first));
        break;
      case OFTReal:
        featAttrs.insert(fName, obj->GetFieldAsDouble(fldDef.first));
        break;
      case OFTIntegerList: {
        const int* ivals = obj->GetFieldAsIntegerList(fldDef.first, &fldListCount);
        for (int i = 0; i < fldListCount; i++)
          lst.push_back(ivals[i]);
        featAttrs.insert(fName, QVariant(lst));
        break;
      }
      case OFTRealList: {
          const double* dvals = obj->GetFieldAsDoubleList(fldDef.first, &fldListCount);
          for (int i = 0; i < fldListCount; i++)
            lst.push_back(dvals[i]);
          featAttrs.insert(fName, QVariant(lst));
          break;
      }
      case OFTStringList:
      case OFTDate:
      case OFTTime:
      case OFTDateTime:
      case OFTBinary:
      case OFTWideString:
      case OFTWideStringList:
        break;
    }
  }

  return featAttrs;
}


extern


bool Chart::readLayer(OGRLayer* poLayer, S52References* ref, OGRDataSource* ds) {
  QString layer_name = QString(poLayer->GetName());
  qDebug() << "Reading" << layer_name << QDateTime::currentDateTime();
  QSet<QString> debugFinalRastRules;

  OGRFeature* poFeature = nullptr;

  AreaLayer* area_layer = new AreaLayer();
  LineLayer* line_layer = new LineLayer();
  MarkLayer* mark_layer = new MarkLayer();


  QMap<QString, std::pair<int, OGRFieldType>> fields;
  auto lrDfn = poLayer->GetLayerDefn();
  for (int fldId = 0; fldId < lrDfn->GetFieldCount(); fldId++) {
    auto fldDfn = lrDfn->GetFieldDefn(fldId);
    //qDebug() << fldDfn->GetNameRef()
    //         << fldDfn->GetFieldTypeName(fldDfn->GetType());
    fields.insert(fldDfn->GetNameRef(), std::pair<int, OGRFieldType>(fldId, fldDfn->GetType()));
  }

  while( (poFeature = poLayer->GetNextFeature()) != nullptr ) {
    QString objName = poFeature->GetDefnRef()->GetName();

    for (int i = 0; i < poFeature->GetGeomFieldCount(); i++) {
      OGRGeometry* geom = poFeature->GetGeomFieldRef(i);
      OGRwkbGeometryType geom_type = geom->getGeometryType();

      floatingATONArray.clear();
      rigidATONArray.clear();
      if (geom_type == wkbPoint) {
        if (objName == "LITFLT" || objName == "LITVES" || objName.left(3) == "BOY")
          floatingATONArray << poFeature->GetFieldAsInteger("RCID");

        if (objName.left(3) == "BCN")
          rigidATONArray << poFeature->GetFieldAsInteger("RCID");
      }

      LookUpTable tbl = LookUpTable::PAPER_CHART;

      switch (geom_type) {
        case wkbLineString:
          tbl = LookUpTable::LINES;
          break;
        case wkbPoint: {
          tbl = CONST_SYMB_LOOKUP;
          break;
        }
        case wkbPolygon:
          tbl = CONST_AREA_LOOKUP;
          break;
        default:
          break;
      }

      auto featAttrs = getOGRFeatureAttributes(poFeature, fields);

      LookUp lp = ref->findBestLookUp(layer_name, featAttrs, tbl);
      if (lp.RCID == -1)
        continue;

      // Expand cond symb
      QStringList extraInstr;
      for (auto instr: lp.INST) {
        RastRuleType type = RAST_RULE_TYPE_MAP.value(instr.left(2), RastRuleType::NONE);

        if (type == RastRuleType::CND_SY) {
          QString symbName = instr.split("(")[1].split(")")[0];
          QString exp = expandCondSymb( symbName
                                      , poFeature
                                      , geom
                                      , &lp
                                      , ds
                                      , ref
                                      , featAttrs
                                      , _m_next_safe_cnt
                                      , floatingATONArray
                                      , rigidATONArray);
          extraInstr << exp.split(";", QString::SkipEmptyParts);
        }
      }

      lp.INST << extraInstr;
      //qDebug() << "final INSTR " << lp.INST;

      for (auto instr: lp.INST) {
        debugFinalRastRules << instr;

        RastRuleType type = RAST_RULE_TYPE_MAP.value(instr.left(2), RastRuleType::NONE);

        switch (type) {

          //Simple point symbol, example: SY(CHINFO06)
          case RastRuleType::SYM_PT: {
            if (geom_type == wkbPoint) {
              OGRPoint* p = static_cast<OGRPoint*>(geom);
              mark_layer->symbol_refs.push_back(instr.split("(")[1].split(")")[0]);
              mark_layer->disp_prio.push_back(static_cast<float>(lp.DPRI));
              mark_layer->points.push_back(static_cast<float>(p->getY()));
              mark_layer->points.push_back(static_cast<float>(p->getX()));
              //qDebug() << "add mark" << instr.split("(")[1].split(")")[0] << p->getX() << p->getY();
            }
            break;
          }

          // Simple line, example: LS(DASH,1,CHGRD)
          case RastRuleType::SIM_LN: {
            QStringList instr_ls = (instr.split("(")[1].split(")")[0]).split(",");
            QString ptrn_ref = instr_ls[0];
            QString col_ref = instr_ls[2];

            if (geom_type == wkbPolygon) {
              OGRPolygon* poly = static_cast<OGRPolygon*>(geom);
              if (!addLineToLayer(line_layer, ptrn_ref, col_ref, lp.DPRI, poly->getExteriorRing()))
                return false;

              for(int iir = 0 ; iir < poly->getNumInteriorRings(); iir++) {
                if (!addLineToLayer(line_layer, ptrn_ref, col_ref, lp.DPRI, poly->getInteriorRing(iir)))
                  return false;
              }
            }

            if (geom_type == wkbLineString) {
              OGRLineString* line = static_cast<OGRLineString*>(geom);
              if (!addLineToLayer(line_layer, instr_ls[0], instr_ls[2], lp.DPRI, line))
                return false;
            }

            break;
          }

          // Pattern line, example: LC(CBLSUB06)
          case RastRuleType::COM_LN: {
            QString ptrn_ref = instr.split("(")[1].split(")")[0];

            if (geom_type == wkbPolygon) {
              OGRPolygon* poly = static_cast<OGRPolygon*>(geom);
              OGRLineString* ring = static_cast<OGRLineString*>(poly->getExteriorRing());

              if (!addLineToLayer(line_layer, ptrn_ref, "CHBLK", lp.DPRI, ring))
                return false;

              for(int iir = 0 ; iir < poly->getNumInteriorRings(); iir++)
                if (!addLineToLayer(line_layer, ptrn_ref, "CHBLK", lp.DPRI, poly->getInteriorRing(iir)))
                  return false;
            }

            if (geom_type == wkbLineString)
              if (!addLineToLayer(line_layer, ptrn_ref, "CHBLK", lp.DPRI, static_cast<OGRLineString*>(geom)))
                return false;

            break;
          }

          // Simple spatial area, example: AC(DEPDW)
          case RastRuleType::ARE_CO: {
            QString col_ref = instr.split("(")[1].split(")")[0];

            if (geom_type == wkbPolygon)
              if (!addAreaToLayer(area_layer, "", col_ref, lp.DPRI, static_cast<OGRPolygon*>(geom)))
                return false;

            break;
          }

          // Pattern spatial area, example: AP(FOULAR01)
          case RastRuleType::ARE_PA:  {
            QString ptrn_ref = instr.split("(")[1].split(")")[0];

            if (geom_type == wkbPolygon)
              if (!addAreaToLayer(area_layer, ptrn_ref, "CHBLK", lp.DPRI, static_cast<OGRPolygon*>(geom)))
                return false;

            break;
          }

          // SHOWTEXT
          case RastRuleType::TXT_TX:  //TX
          case RastRuleType::TXT_TE:  //TE
          case RastRuleType::MUL_SG:  //MP, Special Case for MultPoint Soundings
          case RastRuleType::ARC_2C:  //CA, Special Case for Circular Arc,  (opencpn private)
          case RastRuleType::NONE:
          case RastRuleType::CND_SY:  //CS, Conditional point symbol
            break;
        }
      }
    }

    OGRFeature::DestroyFeature( poFeature );
  }

  if (area_layer->triangles.size() > 0)
    area_layers[layer_name] = area_layer;

  if (line_layer->points.size() > 0)
    line_layers[layer_name] = line_layer;

  if (mark_layer->points.size() > 0)
    mark_layers[layer_name] = mark_layer;

  for (QString rule: debugFinalRastRules)
    qDebug() << rule;

  return true;
}


bool Chart::addLineToLayer(LineLayer* layer, const QString& ptrn_ref, const QString& col_ref, ChartDispPrio dpri, OGRLineString* line) {
  layer->pattern_refs.push_back(ptrn_ref);
  layer->color_inds.push_back(_ref->getColorIndex(col_ref));
  layer->disp_prio.push_back(static_cast<int>(dpri));
  layer->start_inds.push_back(layer->points.size());

  return readOGRLine(line, layer->points, layer->distances);
}

bool Chart::addAreaToLayer(AreaLayer* layer, const QString& ptrn_ref, const QString& col_ref, ChartDispPrio dpri, OGRPolygon* poly) {
  layer->pattern_refs.push_back(ptrn_ref);
  layer->disp_prio.push_back(static_cast<int>(dpri));
  layer->color_inds.push_back(_ref->getColorIndex(col_ref));
  layer->disp_prio.push_back(static_cast<float>(dpri));
  layer->start_inds.push_back(layer->triangles.size());

  return readOGRPolygon(poly, layer->triangles);
}


bool Chart::readOGRPolygon(OGRPolygon* poGeom, std::vector<float> &triangles) {
  // Make a quick sanity check of the polygon coherence
  // ----------------------------------------------------------------
  bool b_ok = true;
  OGRLineString *tls = poGeom->getExteriorRing();

  if(!tls) {
    b_ok = false;
  }
  else {
    int tnpta  = poGeom->getExteriorRing()->getNumPoints();
    if(tnpta < 3)
      b_ok = false;
  }

  for( int iir = 0; iir < poGeom->getNumInteriorRings(); iir++) {
    int tnptr = poGeom->getInteriorRing(iir)->getNumPoints();
    if(tnptr < 3)
      b_ok = false;
  }

  if( !b_ok )
    return false;
  // ----------------------------------------------------------------

  int iir, ip;

  // PolyGeo BBox
  OGREnvelope Envelope;
  poGeom->getEnvelope(&Envelope);
  // xmin = Envelope.MinX;
  // ymin = Envelope.MinY;
  // xmax = Envelope.MaxX;
  // ymax = Envelope.MaxY;


  // Get total number of contours
  int m_ncnt = 1;                             // always exterior ring
  int nint = poGeom->getNumInteriorRings();   // interior rings
  m_ncnt += nint;

  // Allocate cntr array
  int* cntr = static_cast<int *>( malloc(static_cast<uint>(m_ncnt) * sizeof(int)) );

  // Get total number of points(vertices)
  int npta  = poGeom->getExteriorRing()->getNumPoints();
  npta += 2;                                  // fluff

  for( iir=0 ; iir < nint ; iir++) {
    int nptr = poGeom->getInteriorRing(iir)->getNumPoints();
    npta += nptr + 2;
  }

  point_t* geoPt = static_cast<point_t*>(calloc(static_cast<uint>(npta + 1) * sizeof(point_t), 1));     // vertex array

  // Create input structures

  // Exterior Ring
  int npte  = poGeom->getExteriorRing()->getNumPoints();
  cntr[0] = npte;

  point_t* ppt = geoPt;
  ppt->x = 0.; ppt->y = 0.;
  ppt++;                            // vertex 0 is unused

  //  Check and account for winding direction of ring
  bool cw = !(poGeom->getExteriorRing()->isClockwise() == 0);

  double x0, y0, x, y;
  OGRPoint p;

  if (cw) {
    poGeom->getExteriorRing()->getPoint(0, &p);
    x0 = p.getX();
    y0 = p.getY();
  } else {
    poGeom->getExteriorRing()->getPoint(npte-1, &p);
    x0 = p.getX();
    y0 = p.getY();
  }

  // Transcribe points to vertex array, in proper order with no duplicates
  for(ip = 0; ip < npte; ip++) {
    int pidx;
    if(cw)
      pidx = npte - ip - 1;
    else
      pidx = ip;

    poGeom->getExteriorRing()->getPoint(pidx, &p);
    x = p.getX();
    y = p.getY();

    if((fabs(x - x0) > EQUAL_EPS) || (fabs(y - y0) > EQUAL_EPS)) {
      ppt->x = x;
      ppt->y = y;
      ppt++;
    } else
      cntr[0]--;

    x0 = x;
    y0 = y;
  }

  // Now the interior contours
  for(iir = 0; iir < nint; iir++) {
    int npti = poGeom->getInteriorRing(iir)->getNumPoints();
    cntr[iir + 1] = npti;

    // Check and account for winding direction of ring
    bool cw = !(poGeom->getInteriorRing(iir)->isClockwise() == 0);
    if(!cw) {
      poGeom->getInteriorRing(iir)->getPoint(0, &p);
      x0 = p.getX();
      y0 = p.getY();
    } else {
      poGeom->getInteriorRing(iir)->getPoint(npti-1, &p);
      x0 = p.getX();
      y0 = p.getY();
    }

    // Transcribe points to vertex array, in proper order with no duplicates
    for(int ip = 0 ; ip < npti ; ip++) {
      OGRPoint p;
      int pidx;

      if(!cw)                               // interior contours must be cw
        pidx = npti - ip - 1;
      else
        pidx = ip;

      poGeom->getInteriorRing(iir)->getPoint(pidx, &p);
      x = p.getX();
      y = p.getY();

      if((fabs(x - x0) > EQUAL_EPS) || (fabs(y - y0) > EQUAL_EPS)) {
        ppt->x = x;
        ppt->y = y;
        ppt++;
      } else
        cntr[iir+1]--;

      x0 = x;
      y0 = y;
    }
  }

  polyout* polys = triangulate_polygon(m_ncnt, cntr, (double(*)[2])(geoPt));
  polyout* pck = polys;

  while(pck != nullptr) {
    if(pck->is_valid) {
      int *ivs = pck->vertex_index_list;

      if (pck->nvert != 3)
        continue;

      for(int i = 0 ; i < 3 ; i++) {
        int ivp = ivs[i];
        triangles.push_back(static_cast<float>(geoPt[ivp].y));
        triangles.push_back(static_cast<float>(geoPt[ivp].x));
      }
    }

    pck = static_cast<polyout*>(pck->poly_next);
  }

  delete polys;
  return true;
}

bool Chart::readOGRLine(OGRLineString* poGeom, std::vector<float> &ps, std::vector<double> &distances) {
  int point_count = poGeom->getNumPoints();

  if(point_count < 2)
    return false;

  OGRPoint p;
  for( int i = 0; i < point_count; i++ ) {
    poGeom->getPoint(i, &p);

    ps.push_back(static_cast<float>(p.getY()));
    ps.push_back(static_cast<float>(p.getX()));

    if (i != 0)
      distances.push_back( GCDistance( static_cast<double>(ps[ps.size() - 4])
                                     , static_cast<double>(ps[ps.size() - 3])
                                     , static_cast<double>(ps[ps.size() - 2])
                                     , static_cast<double>(ps[ps.size() - 1]) ) );
    else
      distances.push_back(0);
  }

  return true;
}
