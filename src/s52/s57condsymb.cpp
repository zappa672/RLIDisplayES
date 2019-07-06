// Based on OpenCPN s52cnsy.cpp

#include <QDebug>
#include <QMap>
#include <QVariant>

#include "s57condsymb.h"
#include "../common/rlistate.h"


const static RLIDepthUnit m_nDepthUnitDisplay = RLIDepthUnit::METER; // feet/meters

const static double CONST_SHALLOW_DEPTH = 5.0;
const static double CONST_SAFETY_DEPTH  = 10.0;
const static double CONST_DEEP_DEPTH    = 25.0;



inline QVariant getDoubleField(OGRFeature* obj, const char* fieldName) {
  if (obj->GetFieldIndex(fieldName) >= 0)
    return obj->GetFieldAsDouble(fieldName);
  else
    return QVariant(QVariant::Double);
}

inline QVariant getIntField(OGRFeature* obj, const char* fieldName) {
  if (obj->GetFieldIndex(fieldName) >= 0)
    return obj->GetFieldAsInteger(fieldName);
  else
    return QVariant(QVariant::Int);
}

inline QVariant getStringField(OGRFeature* obj, const char* fieldName) {
  if (obj->GetFieldIndex(fieldName) >= 0)
    return obj->GetFieldAsString(fieldName);
  else
    return QVariant(QVariant::String);
}




// Put a string of comma delimited number in a QSet.
static QSet<int> parseIntList(const QString str) {
  QSet<int> res;
  QStringList lst = str.split(",");
  for (QString s: lst)
    res << s.toInt();
  return res;
}


// Remarks: S-57 Appendix B1 Annex A requires in Section 6 that areas of rocks be
// encoded as area obstruction, and that area OBSTRNs and area WRECKS
// be covered by either group 1 object DEPARE or group 1 object UNSARE.
// If the value of the attribute VALSOU for an area OBSTRN or WRECKS
// is missing, the DRVAL1 of an underlying DEPARE is the preferred default
// for establishing a depth value. This procedure either finds the shallowest
// DRVAL1 of the one or more underlying DEPAREs, or returns an
// "unknown"" depth value to the main procedure for the next default
// procedure.
// NOTE: UNSARE test is useless since least_depth is already UNKNOWN
static QVariant _DEPVAL01(OGRFeature* obj, double least_depth) {
  // S57_geo *geoTmp = geo;
  //
  // // NOTE: the geo list is unchange (_UDWHAZ03 will unlink geo)
  // while (NULL != (geoTmp = S57_nextObj(geoTmp))) {
  //   GString *objlstr = S57_getAttVal(geoTmp, "OBJL");
  //   int      objl    = (NULL == objlstr) ? 0 : atoi(objlstr->str);
  //
  //   // get area DEPARE  that intersect this area
  //   if (DEPARE==objl && LINES_T==S57_getObjtype(geo)) {
  //     GString *drval1str = S57_getAttVal(geoTmp, "DRVAL1");
  //     double   drval1    = (NULL == drval1str) ? 9.0 : atof(drval1str->str);
  //
  //     if (NULL != drval1str)
  //       if (UNKNOWN==least_depth || least_depth<drval1)
  //         least_depth = drval1;
  //   }
  //}

  return QVariant(QVariant::Double);
}

// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol as hazardous objects
// and put in IMO category DISPLAYBASE (see (3), App.2, 1.3). This task
// is performed by this conditional symbology procedure.
static QString _UDWHAZ03(OGRFeature* obj, QVariant depth_value, LookUp* lp, OGRDataSource* ds) {
  QString udwhaz03str;

  bool danger = false;
  QVariant expsou = getIntField(obj, "EXPSOU");

  if (depth_value.isNull()) {
    if (!expsou.isNull() && expsou.toInt() != 1)
      danger = true;
  }

  if (!danger && (expsou == 1 || depth_value <= CONST_SAFETY_DEPTH)) {
    // that intersect this point/line/area for OBSTRN04
    // that intersect this point/area      for WRECKS02


      /*
    // get area DEPARE & DRGARE that intersect this point/line/area
    ListOfS57Obj *pobj_list = NULL;

    if (obj->m_chart_context->chart)
      pobj_list = obj->m_chart_context->chart->GetAssociatedObjects(obj);
    else {
      danger = false;
      //wxString *ret_str = new wxString(udwhaz03str);
      //return ret_str;
    }

    if (pobj_list) {
      wxListOfS57ObjNode *node = pobj_list->GetFirst();
      while(node) {
        S57Obj *ptest_obj = node->GetData();

        if(GEO_LINE == ptest_obj->Primitive_type) {
          double drval2 = 0.0;
          GetDoubleAttr(ptest_obj, "DRVAL2", drval2);

          if (drval2 < safety_contour) {
            danger = TRUE;
            break;
          }
        } else {
          double drval1 = 0.0;
          GetDoubleAttr(ptest_obj, "DRVAL1", drval1);

          if (drval1 >= safety_contour && expsou != 1) {
            danger = TRUE;
            break;
          }
        }

        node = node->GetNext();
      }

      delete pobj_list;
    }
    */
  }

  if (danger) {
    QVariant watlev = getIntField(obj, "WATLEV");

    if((watlev == 1) || (watlev == 2)) {
      // dry
      // udwhaz03str = _T(";OP(--D14050)");
    } else {
      udwhaz03str = ";SY(ISODGR51)";
    }

    // Move this object to DisplayBase category
    lp->DISC = ChartDisplayCat::DISP_CAT_DISPLAYBASE;
  }

  return udwhaz03str;
}


// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// This procedure retrieves any QUALTY (ne QUAPOS) attributes, and returns the
// appropriate symbols to the calling procedure.
QString CSQUAPNT01(OGRFeature* obj) {
  QVariant quapos = getIntField(obj, "QUAPOS");

  if (!quapos.isNull())
    switch(quapos.toInt()) {
    case 4:
      return ";SY(QUAPOS01)";   // "PA"
    case 5:
      return ";SY(QUAPOS02)";   // "PD"
    case 7:
    case 8:
      return ";SY(QUAPOS03)";   // "REP"
    default:
      return ";SY(LOWACC03)";   // "?"
    }

  return "";
}


// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// This procedure retrieves any QUALTY (ne QUAPOS) attributes, and returns the
// appropriate symbols to the calling procedure.
static QString QUAPNT01(OGRFeature* obj) {
  return CSQUAPNT01(obj);
}


// Remarks: Soundings differ from plain text because they have to be readable under all
// circumstances and their digits are placed according to special rules. This
// conditional symbology procedure accesses a set of carefully designed
// sounding symbols provided by the symbol library and composes them to
// sounding labels. It symbolizes swept depth and it also symbolizes for low
// reliability as indicated by attributes QUASOU and QUAPOS.
QString _SNDFRM02(OGRFeature* obj, double depth_value_in) {
  qDebug() << "-";

  qDebug() << "SNDFRM02";

  QString sndfrm02;  
  QString symbol_prefix;

  QSet<int> tecsou = parseIntList(getStringField(obj, "TECSOU").toString());
  QSet<int> quasou = parseIntList(getStringField(obj, "QUASOU").toString());
  QSet<int> status = parseIntList(getStringField(obj, "STATUS").toString());

  double   leading_digit    = 0.0;


  // TODO:
  double safety_depth = CONST_SAFETY_DEPTH;//S52_getMarinerParam(S52_MAR_SAFETY_DEPTH);

  // Do the math to convert soundings to ft/metres/fathoms on request
  double depth_value = depth_value_in;

  // If the sounding value from the ENC (or SENC) is bogus, so state
  if (depth_value_in > 40000.)
    depth_value = 99999.;
  if (depth_value_in < -1000.)
    depth_value = 0.;

  switch(m_nDepthUnitDisplay) {
  case RLIDepthUnit::FEET:
    depth_value = depth_value   * 3 * 39.37 / 36;
    safety_depth = safety_depth * 3 * 39.37 / 36;
    break;
  case RLIDepthUnit::FATHOM:
    depth_value = depth_value   * 3 * 39.37 / (36 * 6);
    safety_depth = safety_depth * 3 * 39.37 / (36 * 6);
    break;
  default:
    break;
  }

  // FIXME: test to fix the rounding error (!?)
  depth_value  += (depth_value > 0.0) ? 0.01 : -0.01;
  leading_digit = static_cast<int>(fabs(depth_value));

  if (depth_value <= safety_depth)            //S52_getMarinerParam(S52_MAR_SAFETY_DEPTH)
    symbol_prefix = "SOUNDS";
  else
    symbol_prefix = "SOUNDG";

  if (tecsou.contains(6))
    sndfrm02.append(";SY(" + symbol_prefix + "B1)");


  if ( quasou.intersect(QSet<int> { 3, 4, 5, 10, 11}).size() > 0 || status.contains(22) )
    sndfrm02.append(";SY(" + symbol_prefix + "C2)");
  else
    if (obj->GetFieldIndex("QUAPOS") >= 0) {
      int quapos = obj->GetFieldAsInteger("QUAPOS");
      if (2 <= quapos && quapos < 10)
        sndfrm02.append(";SY(" + symbol_prefix + "C2)");
    }

  // Continuation A
  if (fabs(depth_value) < 10.0) {
    // If showing as "feet", round off to one digit only
    if( (m_nDepthUnitDisplay == RLIDepthUnit::FEET) && (depth_value > 0) ) {
      depth_value = std::round(depth_value);
      leading_digit = static_cast<int>(depth_value);
    }

    if (depth_value < 10.0) {
      // can be above water (negative)
      int fraction = static_cast<int>(fabs((fabs(depth_value) - leading_digit)*10));

      sndfrm02.append(";SY(" + symbol_prefix + "1" + QString::number(leading_digit)[0] + ")");
      if (fraction > 0) {
        sndfrm02.append(";SY(" + symbol_prefix + "5" + QString::number(fraction)[0] + ")");
      }

      // above sea level (negative)
      if (depth_value < 0.0)
        sndfrm02.append(";SY(" + symbol_prefix + "A1)");

      return sndfrm02;
    }
  }

  if (fabs(depth_value) < 31.0) {
    bool b_2digit = false;
    double depth_value_pos = fabs(depth_value);

    // If showing as "feet", round off to two digits only
    if ( (m_nDepthUnitDisplay == RLIDepthUnit::FEET) && (depth_value_pos > 0) ) {
      depth_value = std::round(depth_value);
      leading_digit = static_cast<int>(depth_value_pos);
      b_2digit = true;
    }

    double fraction = fabs(depth_value_pos - floor(leading_digit));

    if (fraction != 0.0) {
      fraction = fraction * 10;

      if (leading_digit >= 10.0)
        sndfrm02.append(";SY(" + symbol_prefix + "2" + QString::number(leading_digit/10)[0] + ")");

      double digit1 = floor(leading_digit / 10);
      int digit2 = static_cast<int>(floor(leading_digit - (digit1 * 10)));
      sndfrm02.append(";SY(" + symbol_prefix + "1" + QString::number(digit2)[0] + ")");

      if (!b_2digit)
        if (static_cast<int>(fraction) > 0)
          sndfrm02.append(";SY(" + symbol_prefix + "5" + QString::number(fraction)[0] + ")");

      if (depth_value < 0.0)
        sndfrm02.append(";SY(" + symbol_prefix + "A1)");

      return sndfrm02;
    }
  }

  // Continuation B
  if (fabs(depth_value) < 100.0) {
      QString depthStr = QString::number(fabs(leading_digit));

      if (depth_value < 0.0) {
        sndfrm02.append(";SY(" + symbol_prefix + "2" + depthStr[0] + ")");
        sndfrm02.append(";SY(" + symbol_prefix + "1" + depthStr[1] + ")");
        sndfrm02.append(";SY(" + symbol_prefix + "A1)");
      } else {
        sndfrm02.append(";SY(" + symbol_prefix + "1" + depthStr[0] + ")");
        sndfrm02.append(";SY(" + symbol_prefix + "0" + depthStr[1] + ")");
      }

      return sndfrm02;
  }

  if (depth_value < 1000.0) {
    QString depthStr = QString::number(fabs(leading_digit));

    sndfrm02.append(";SY(" + symbol_prefix + "2" + depthStr[0] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "1" + depthStr[1] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "0" + depthStr[2] + ")");

    return sndfrm02;
  }

  if (depth_value < 10000.0) {      
    QString depthStr = QString::number(fabs(leading_digit));

    sndfrm02.append(";SY(" + symbol_prefix + "2" + depthStr[0] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "1" + depthStr[1] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "0" + depthStr[2] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "4" + depthStr[3] + ")");

    return sndfrm02;
  }

  // Continuation C
  {
    QString depthStr = QString::number(fabs(leading_digit));

    sndfrm02.append(";SY(" + symbol_prefix + "3" + depthStr[0] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "2" + depthStr[1] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "1" + depthStr[2] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "0" + depthStr[3] + ")");
    sndfrm02.append(";SY(" + symbol_prefix + "4" + depthStr[4] + ")");

    return sndfrm02;
  }
}


// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol and put in IMO
// category DISPLAYBASE (see (3), App.2, 1.3). This task is performed
// by the sub-procedure "UDWHAZ03" which is called by this symbology
// procedure. Objects of the class "under water rock" are handled by this
// routine as well to ensure a consistent symbolization of isolated dangers on
// the seabed.
static QString OBSTRN04(OGRFeature* obj, OGRwkbGeometryType geomType, LookUp* lp, OGRDataSource* ds) {
  qDebug() << "--";
  qDebug() << "OBSTRN04";

  QString obstrn04str;
  QVariant udwhaz03str = QVariant(QVariant::String);

  QVariant catobs = getIntField(obj, "CATOBS");
  QVariant watlev = getIntField(obj, "WATLEV");
  QVariant expsou = getIntField(obj, "EXPSOU");
  QVariant valsou = getDoubleField(obj, "VALSOU");

  qDebug() << "valsou" << valsou << valsou.isNull();
  qDebug() << "catobs" << catobs << catobs.isNull();
  qDebug() << "watlev" << watlev << watlev.isNull();
  qDebug() << "expsou" << expsou << expsou.isNull();

  QVariant depth_value = QVariant(QVariant::Double);
  QVariant least_depth = QVariant(QVariant::Double);

  QString sndfrm02str;
  QVariant quapnt01str = QVariant(QVariant::String);

  if (!valsou.isNull()) {
    depth_value = valsou;
    sndfrm02str = _SNDFRM02(obj, valsou.toDouble());
  } else {
    if (geomType == wkbPolygon)
      least_depth = _DEPVAL01(obj, least_depth.toDouble());

    if (!least_depth.isNull())
      depth_value = least_depth;
    else
      if (expsou.toInt() != 1) {
        if (catobs == 6)
          depth_value = 0.01;
        else if (watlev.isNull() || watlev.toDouble() == 0.0) // default
          depth_value = -15.0;
        else {
          switch (watlev.toInt()) {
            case 5:
              depth_value = 0.0;
              break;
            case 3:
              depth_value = 0.01;
              break;
            default :
              depth_value = -15.0;
              break;
          }
        }
      }
  }

  udwhaz03str = _UDWHAZ03(obj, depth_value.toDouble(), lp, ds);

  if (geomType == wkbPoint) {
    // Continuation A
    int sounding    = FALSE;
    quapnt01str = CSQUAPNT01(obj);

    if (!udwhaz03str.isNull() && udwhaz03str.toString().length() > 0) {
      obstrn04str.append(udwhaz03str.toString());
      obstrn04str.append(quapnt01str.toString());

      goto end;
    }

    if (!valsou.isNull()) {
      if (valsou <= 20.0) {
        if (!strncmp(obj->GetDefnRef()->GetName(), "UWTROC", 6)) {
          if (watlev.isNull()) {  // default
            obstrn04str.append(";SY(DANGER51)");
            sounding = TRUE;
          } else {
            switch (watlev.toInt()) {
              case 3:
                obstrn04str.append(";SY(DANGER51)");
                sounding = TRUE ;
                break;
              case 4:
              case 5:
                obstrn04str.append(";SY(UWTROC04)");
                sounding = FALSE;
                break;
              default :
                obstrn04str.append(";SY(DANGER51)");
                sounding = TRUE;
                break;
            }
          }
        } else { // OBSTRN
          if (watlev.isNull()) { // default
            obstrn04str.append(";SY(DANGER01)");
            sounding = TRUE;
          } else {
            switch (watlev.toInt()) {
              case 1:
              case 2:
                obstrn04str.append(";SY(LNDARE01)");
                sounding = FALSE;
                break;
              case 3:
                obstrn04str.append(";SY(DANGER52)");
                sounding = TRUE;
                break;
              case 4:
              case 5:
                obstrn04str.append(";SY(DANGER53)");
                sounding = TRUE;
                break;
              default :
                obstrn04str.append(";SY(DANGER51)");
                sounding = TRUE;
                break;
            }
          }
        }
      } else {  // valsou > 20.0
        obstrn04str.append(";SY(DANGER52)");
        sounding = TRUE;
      }
    } else {  // NO valsou
      if (!strncmp(obj->GetDefnRef()->GetName(), "UWTROC", 6)) {
        if (watlev.isNull())  // default
          obstrn04str.append(";SY(UWTROC04)");
        else {
          switch (watlev.toInt()) {
            case 2:
              obstrn04str.append(";SY(LNDARE01)");
              break;
            case 3:
              obstrn04str.append(";SY(UWTROC03)");
              break;
            default:
              obstrn04str.append(";SY(UWTROC04)");
              break;
          }
        }
      } else { // OBSTRN
        if (watlev.isNull()) // default
          obstrn04str = ";SY(OBSTRN01)";
        else {
          switch (watlev.toInt()) {
            case 1:
              obstrn04str.append(";SY(OBSTRN11)");
              break;
            case 2:
              obstrn04str.append(";SY(OBSTRN11)");
              break;
            case 3:
              obstrn04str.append(";SY(OBSTRN01)");
              break;
            case 4:
              obstrn04str.append(";SY(OBSTRN03)");
              break;
            case 5:
              obstrn04str.append(";SY(OBSTRN03)");
              break;
            default:
              obstrn04str.append(";SY(OBSTRN01)");
              break;
          }
        }
      }
    }

    if (sounding)
      obstrn04str.append(sndfrm02str);

    obstrn04str.append(quapnt01str.toString());

    goto end;

  } else { // if geopoint
    if (geomType == wkbLineString) {
      // Continuation B

      quapnt01str = CSQUAPNT01(obj);

      if (!quapnt01str.isNull() && quapnt01str.toString().length() > 1) {
        long quapos = quapnt01str.toLongLong();
        //quapnt01str->ToLong(&quapos);
        if (2 <= quapos && quapos < 10) {
          if (udwhaz03str.toString().length())
            obstrn04str.append(";LC(LOWACC41)");
          else
            obstrn04str.append(";LC(LOWACC31)");
          }

          goto end;
        }

      if (udwhaz03str.toString().length()) {
        obstrn04str.append("LS(DOTT,2,CHBLK)");
        goto end;
      }

      if (nan("") != valsou) {
        if (valsou <= 20.0)
          obstrn04str.append(";LS(DOTT,2,CHBLK)");
        else
          obstrn04str.append(";LS(DASH,2,CHBLK)");
      } else
        obstrn04str.append(";LS(DOTT,2,CHBLK)");


      if (udwhaz03str.toString().length()) {
          //  Show the isolated danger symbol at the midpoint of the line
      } else {
        if (nan("") != valsou)
          if (valsou <= 20.0)
            obstrn04str.append(sndfrm02str);
      }
    } else { // Area feature
      quapnt01str = CSQUAPNT01(obj);

      if (0 != udwhaz03str.toString().length()) {
        obstrn04str.append(";AC(DEPVS);AP(FOULAR01)");
        obstrn04str.append(";LS(DOTT,2,CHBLK)");
        obstrn04str.append(udwhaz03str.toString());
        obstrn04str.append(quapnt01str.toString());

        goto end;
      }

      if (nan("") != valsou) {
        // BUG in CA49995B.000 if we get here because there is no color
        // beside NODATA (ie there is a hole in group 1 area!)
        //g_string_append(obstrn04, ";AC(UINFR)");

        if (valsou <= 20.0)
          obstrn04str.append(";LS(DOTT,2,CHBLK)");
        else
          obstrn04str.append(";LS(DASH,2,CHBLK)");

        obstrn04str.append(sndfrm02str);
      } else {
        if (watlev.isNull())   // default
          obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");
        else {
          switch (watlev.toInt()) {
            case 1:
            case 2:
              obstrn04str.append(";AC(CHBRN);LS(SOLD,2,CSTLN)");
              break;
            case 4:
              obstrn04str.append(";AC(DEPIT);LS(DASH,2,CSTLN)");
              break;
            case 5:
            case 3: {
              int catobs = -9;
              if (obj->GetFieldIndex("CATOBS") >= 0)
                catobs = obj->GetFieldAsInteger("CATOBS");

              if (6 == catobs)
                obstrn04str.append(";AC(DEPVS);AP(FOULAR01);LS(DOTT,2,CHBLK)");
              else
                obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");
            }
            break;
            default: obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");  break;
          }
        }
      }

      obstrn04str.append(quapnt01str.toString());
      goto end;
    }     // area
  }

end:
  obstrn04str.append(';');

  qDebug() << "--";

  return obstrn04str;
}

QString expandCondSymb(QString cs, OGRFeature* obj, OGRwkbGeometryType geomType, LookUp* lp, OGRDataSource* ds) {
  if (cs == "OBSTRN04")
    return OBSTRN04(obj, geomType, lp, ds);

  /*
  if (cs == "CLRLIN01")
    return CLRLIN01(obj, geomType, lp);
  if (cs == "DATCVR01")
    return DATCVR01(obj, geomType, lp);
  if (cs == "DEPARE01")
    return DEPARE01(obj, geomType, lp);
  if (cs == "DEPARE02")
    return DEPARE01(obj, geomType, lp);                 // new in PLIB 3_3, opencpn defaults to DEPARE01
  if (cs == "DEPCNT02")
    return DEPCNT02(obj, geomType, lp);
  if (cs == "DEPVAL01")
    return DEPVAL01(obj, geomType, lp);
  if (cs == "LEGLIN02")
    return LEGLIN02(obj, geomType, lp);
  if (cs == "LIGHTS05")
    return LIGHTS05(obj, geomType, lp);                 // new in PLIB 3_3, replaces LIGHTS04
  if (cs == "LITDSN01")
    return LITDSN01(obj, geomType, lp);
  if (cs == "OWNSHP02")
    return OWNSHP02(obj, geomType, lp);
  if (cs == "PASTRK01")
    return PASTRK01(obj, geomType, lp);
  if (cs == "QUAPOS01")
    return QUAPOS01(obj, geomType, lp);
  if (cs == "QUALIN01")
    return QUALIN01(obj, geomType, lp);
  if (cs == "QUAPNT01")
    return QUAPNT01(obj, geomType, lp);
  if (cs == "SLCONS03")
    return SLCONS03(obj, geomType, lp);
  if (cs == "RESARE02")
    return RESARE02(obj, geomType, lp);
  if (cs == "RESTRN01")
    return RESTRN01(obj, geomType, lp);
  if (cs == "SEABED01")
    return SEABED01(obj, geomType, lp);
  if (cs == "SOUNDG02")
    return SOUNDG02(obj, geomType, lp);
  if (cs == "TOPMAR01")
    return TOPMAR01(obj, geomType, lp);
  if (cs == "UDWHAZ03")
    return UDWHAZ03(obj, geomType, lp);
  if (cs == "VESSEL01")
    return VESSEL01(obj, geomType, lp);
  if (cs == "VRMEBL01")
    return VRMEBL01(obj, geomType, lp);
  if (cs == "WRECKS02")
    return WRECKS02(obj, geomType, lp);
  if (cs == "SOUNDG03")
    return SOUNDG03(obj, geomType, lp);
  */

  return "";
}
