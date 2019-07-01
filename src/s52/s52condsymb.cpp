// Based on OpenCPN s52cnsy.cpp

#include <QDebug>
#include <QMap>
#include <QVariant>
#include <QString>

#include <ogrsf_frmts.h>
#include "s52references.h"

#define LISTSIZE 32

#ifndef chk_snprintf
#define chk_snprintf(buf, len, fmt, ...) \
{ \
    int r = snprintf(buf, len, fmt, ##__VA_ARGS__); \
    if (r == -1 || r >= len) qDebug() << ("snprint overrun"); \
}
#endif


const static int m_nDepthUnitDisplay = 0; // miles/km

const static double CONST_SAFETY_DEPTH = 20.0;

// shallow_contour_val
//double shc = 2.0;
// safety_contour_val
//double sfc = 20.0;
// deep_contour_val
//double dpc = 30.0;

// Put a string of comma delimited number in an array (buf).
// Return: the number of value in buf.
// Assume: - number < 256,
//         - list size less than buf_size .
// Note: buf is \0 terminated for strpbrk().
static int _parseList(const char *str_in, char *buf) {
  char* str = const_cast<char*>(str_in);
  int i = 0;

  if (nullptr != str && *str != '\0') {
    do {
      if (i >= LISTSIZE-1) {
        printf("OVERFLOW --value in list lost!!\n");
        break;
      }

      buf[i++] = static_cast<char>(atoi(str));

      while(isdigit(*str))
        str++;   // next
    } while (*str++ != '\0');      // skip ',' or exit
  }

  buf[i] = '\0';

  return i;
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
static double _DEPVAL01(OGRFeature* obj, double least_depth) {
  least_depth = nan("");

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

  return least_depth;
}

// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol as hazardous objects
// and put in IMO category DISPLAYBASE (see (3), App.2, 1.3). This task
// is performed by this conditional symbology procedure.
static QString _UDWHAZ03(OGRFeature* obj, double depth_value, LookUp* lp, bool *promote_return) {
    QString udwhaz03str;
    int     danger = FALSE;
    int	    expsou = 0;
    double  safety_contour = CONST_SAFETY_DEPTH; //S52_getMarinerParam(S52_MAR_SAFETY_CONTOUR);
    bool    b_promote = false;

    if (depth_value == nan("")) {
      if (obj->GetFieldIndex("EXPSOU") > 0) {
        expsou = obj->GetFieldAsInteger("EXPSOU");
        if (expsou != 1)
          danger = TRUE;
      }
      danger = TRUE;
    }

    /* TODO: ---------------------------------------------------------------
     * TODO: ---------------------------------------------------------------
     * TODO: ---------------------------------------------------------------
     * TODO: ---------------------------------------------------------------
    if (danger == FALSE && (expsou == 1 || depth_value <= safety_contour)) {
      // that intersect this point/line/area for OBSTRN04
      // that intersect this point/area      for WRECKS02

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
    }
    */

    if (TRUE == danger) {
      int watlev = 0; // Enum 0 invalid
      if (obj->GetFieldIndex("WATLEV") > 0)
        watlev = obj->GetFieldAsInteger("WATLEV");

      if((1 == watlev) || (2 == watlev)) {
        // dry
        // udwhaz03str = _T(";OP(--D14050)");
      } else {
        udwhaz03str = ";SY(ISODGR51)";     //_T(";OP(8OD14010);SY(ISODGR51)");
        // S57_setAtt(geo, "SCAMIN", "INFINITE");
      }

      // Move this object to DisplayBase category
      lp->DISC = ChartDisplayCat::DISP_CAT_DISPLAYBASE;
    }


    if(promote_return)
        *promote_return = b_promote;

    return udwhaz03str;
}


// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// This procedure retrieves any QUALTY (ne QUAPOS) attributes, and returns the
// appropriate symbols to the calling procedure.
QString CSQUAPNT01(OGRFeature* obj) {
  QString quapnt01;
  int accurate  = TRUE;
  int qualty = 10;
  int bquapos = FALSE;
  if (obj->GetFieldIndex("QUAPOS") > 0)
    bquapos = obj->GetFieldAsInteger("QUAPOS");

  if (bquapos) {
    if (2 <= qualty && qualty < 10)
      accurate = FALSE;
  }

  if (!accurate) {
    switch(qualty) {
    case 4:
      quapnt01.append(";SY(QUAPOS01)");
      break;      // "PA"
    case 5:
      quapnt01.append(";SY(QUAPOS02)");
      break;      // "PD"
    case 7:
    case 8:
      quapnt01.append(";SY(QUAPOS03)");
      break;      // "REP"
    default:
      quapnt01.append(";SY(LOWACC03)");
      break;      // "?"
    }
  }

  quapnt01.append('\037');
  return quapnt01;
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
QString SNDFRM02(OGRFeature* obj, double depth_value_in) {
  QString sndfrm02;
  char    temp_str[LISTSIZE] = {'\0'};
  QString symbol_prefix;

  char symbol_prefix_a[200];

  QVariant tecsoustr(QVariant::String);
  if (obj->GetFieldIndex("TECSOU") >= 0)
    tecsoustr = obj->GetFieldAsString("TECSOU");
  char     tecsou[LISTSIZE] = {'\0'};

  QVariant quasoustr(QVariant::String);
  if (obj->GetFieldIndex("QUASOU") >= 0)
    tecsoustr = obj->GetFieldAsString("QUASOU");
  char     quasou[LISTSIZE] = {'\0'};

  QVariant statusstr(QVariant::String);
  if (obj->GetFieldIndex("STATUS") >= 0)
    tecsoustr = obj->GetFieldAsString("STATUS");
  char     status[LISTSIZE] = {'\0'};

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
  case 0:
    depth_value = depth_value   * 3 * 39.37 / 36;              // feet
    safety_depth = safety_depth * 3 * 39.37 / 36;
    break;
  case 2:
    depth_value = depth_value   * 3 * 39.37 / (36 * 6);        // fathoms
    safety_depth = safety_depth * 3 * 39.37 / (36 * 6);
    break;
  default:
    break;
  }

  // FIXME: test to fix the rounding error (!?)
  depth_value  += (depth_value > 0.0)? 0.01: -0.01;
  leading_digit = static_cast<int>(fabs(depth_value));

  if (depth_value <= safety_depth)            //S52_getMarinerParam(S52_MAR_SAFETY_DEPTH)
    symbol_prefix = "SOUNDS";
  else
    symbol_prefix = "SOUNDG";

  strcpy(symbol_prefix_a, symbol_prefix.toStdString().data());

  if (!tecsoustr.isNull()) {
    _parseList(tecsoustr.toString().toStdString().data(), tecsou);
    if (strpbrk(tecsou, "\006")) {
      chk_snprintf(temp_str, LISTSIZE, ";SY(%sB1)", symbol_prefix_a);
      sndfrm02.append(temp_str);
    }
  }

  if (!quasoustr.isNull()) _parseList(quasoustr.toString().toStdString().data(), quasou);
  if (!statusstr.isNull()) _parseList(statusstr.toString().toStdString().data(), status);

  if (strpbrk(quasou, "\003\004\005\010\011") || strpbrk(status, "\022")) {
    chk_snprintf(temp_str, LISTSIZE, ";SY(%sC2)", symbol_prefix_a);
    sndfrm02.append(temp_str);
  } else {
    if (obj->GetFieldIndex("QUAPOS") > 0) {
      int quapos = obj->GetFieldAsInteger("QUAPOS");
      if (2 <= quapos && quapos < 10) {
        chk_snprintf(temp_str, LISTSIZE,";SY(%sC2)", symbol_prefix_a);
        sndfrm02.append(temp_str);
      }
    }
  }

  // Continuation A
  if (fabs(depth_value) < 10.0) {
    // If showing as "feet", round off to one digit only
    if( (m_nDepthUnitDisplay == 0) && (depth_value > 0) ) {
      depth_value = std::round(depth_value);
      leading_digit = static_cast<int>(depth_value);
    }

    if (depth_value < 10.0) {
      // can be above water (negative)
      int fraction = static_cast<int>(fabs((fabs(depth_value) - leading_digit)*10));

      chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, static_cast<int>(fabs(leading_digit)));
      sndfrm02.append(temp_str);
      if (fraction > 0) {
        chk_snprintf(temp_str, LISTSIZE,";SY(%s5%1i)", symbol_prefix_a, fraction);
        sndfrm02.append(temp_str);;
      }

      // above sea level (negative)
      if (depth_value < 0.0) {
        chk_snprintf(temp_str, LISTSIZE,";SY(%sA1)", symbol_prefix_a);
        sndfrm02.append(temp_str);
      }

      goto return_point;
    }
  }

  if (fabs(depth_value) < 31.0) {
    bool b_2digit = false;
    double depth_value_pos = fabs(depth_value);

    // If showing as "feet", round off to two digits only
    if ( (m_nDepthUnitDisplay == 0) && (depth_value_pos > 0) ) {
      depth_value = std::round(depth_value);
      leading_digit = static_cast<int>(depth_value_pos);
      b_2digit = true;
    }

    double fraction = fabs(depth_value_pos - floor(leading_digit));

    if (fraction != 0.0) {
      fraction = fraction * 10;
      if (leading_digit >= 10.0) {
        chk_snprintf(temp_str, LISTSIZE, ";SY(%s2%1i)",symbol_prefix_a, static_cast<int>(leading_digit/10));
        sndfrm02.append(temp_str);
      }

      double first_digit = floor(leading_digit / 10);
      int secnd_digit = static_cast<int>(floor(leading_digit - (first_digit * 10)));
      chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, secnd_digit/*(int)leading_digit*/);
      sndfrm02.append(temp_str);

      if (!b_2digit){
        if (static_cast<int>(fraction) > 0) {
          chk_snprintf(temp_str, LISTSIZE, ";SY(%s5%1i)",symbol_prefix_a, static_cast<int>(fraction));
          sndfrm02.append(temp_str);
        }
      }

      if (depth_value < 0.0) {
        chk_snprintf(temp_str, LISTSIZE, ";SY(%sA1)", symbol_prefix_a);
        sndfrm02.append(temp_str);
      }

      goto return_point;
    }
  }

  // Continuation B
  if (fabs(depth_value) < 100.0) {
    leading_digit = fabs(leading_digit);

    double first_digit = floor(leading_digit / 10);
    double secnd_digit = floor(leading_digit - (first_digit * 10));

    if (depth_value < 0.0) {
      chk_snprintf(temp_str, LISTSIZE, ";SY(%s2%1i)",symbol_prefix_a, static_cast<int>(first_digit));
      sndfrm02.append(temp_str);
      chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, static_cast<int>(secnd_digit));
      sndfrm02.append(temp_str);
      chk_snprintf(temp_str, LISTSIZE, ";SY(%sA1)", symbol_prefix_a);
      sndfrm02.append(temp_str);
    } else {
      chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, static_cast<int>(first_digit));
      sndfrm02.append(temp_str);
      chk_snprintf(temp_str, LISTSIZE, ";SY(%s0%1i)",symbol_prefix_a, static_cast<int>(secnd_digit));
      sndfrm02.append(temp_str);
    }

    goto return_point;
  }

  if (depth_value < 1000.0) {
    double first_digit = floor(leading_digit / 100);
    double secnd_digit = floor((leading_digit - (first_digit * 100)) / 10);
    double third_digit = floor(leading_digit - (first_digit * 100) - (secnd_digit * 10));

    chk_snprintf(temp_str, LISTSIZE, ";SY(%s2%1i)",symbol_prefix_a, static_cast<int>(first_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, static_cast<int>(secnd_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s0%1i)",symbol_prefix_a, static_cast<int>(third_digit));
    sndfrm02.append(temp_str);

    goto return_point;
  }

  if (depth_value < 10000.0) {
    double first_digit = floor(leading_digit / 1000);
    double secnd_digit = floor((leading_digit - (first_digit * 1000)) / 100);
    double third_digit = floor((leading_digit - (first_digit * 1000) - (secnd_digit * 100)) / 10);
    double last_digit  = floor(leading_digit - (first_digit * 1000) - (secnd_digit * 100) - (third_digit * 10)) ;

    chk_snprintf(temp_str, LISTSIZE, ";SY(%s2%1i)",symbol_prefix_a, static_cast<int>(first_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, static_cast<int>(secnd_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s0%1i)",symbol_prefix_a, static_cast<int>(third_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s4%1i)",symbol_prefix_a, static_cast<int>(last_digit));
    sndfrm02.append(temp_str);

    goto return_point;
  }

  // Continuation C
  {
    double first_digit  = floor(leading_digit / 10000);
    double secnd_digit  = floor((leading_digit - (first_digit * 10000)) / 1000);
    double third_digit  = floor((leading_digit - (first_digit * 10000) - (secnd_digit * 1000)) / 100 );
    double fourth_digit = floor((leading_digit - (first_digit * 10000) - (secnd_digit * 1000) - (third_digit * 100)) / 10 ) ;
    double last_digit   = floor(leading_digit - (first_digit * 10000) - (secnd_digit * 1000) - (third_digit * 100) - (fourth_digit * 10)) ;

    chk_snprintf(temp_str, LISTSIZE, ";SY(%s3%1i)",symbol_prefix_a, static_cast<int>(first_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s2%1i)",symbol_prefix_a, static_cast<int>(secnd_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s1%1i)",symbol_prefix_a, static_cast<int>(third_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s0%1i)",symbol_prefix_a, static_cast<int>(fourth_digit));
    sndfrm02.append(temp_str);
    chk_snprintf(temp_str, LISTSIZE, ";SY(%s4%1i)",symbol_prefix_a, static_cast<int>(last_digit));
    sndfrm02.append(temp_str);

    goto return_point;
  }

return_point:
  sndfrm02.append('\037');
  return sndfrm02;
}


// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol and put in IMO
// category DISPLAYBASE (see (3), App.2, 1.3). This task is performed
// by the sub-procedure "UDWHAZ03" which is called by this symbology
// procedure. Objects of the class "under water rock" are handled by this
// routine as well to ensure a consistent symbolization of isolated dangers on
// the seabed.
static QString OBSTRN04(OGRFeature* obj, OGRwkbGeometryType geomType, LookUp* lp) {
  QString obstrn04str;
  QVariant udwhaz03str = QVariant(QVariant::String);
  bool b_promote = false;

  //ObjRazRules *rzRules = (ObjRazRules *)param;
  //S57Obj *obj = rzRules->obj;

  double valsou      = nan("");
  double depth_value = nan("");
  double least_depth = nan("");

  QString sndfrm02str;
  QVariant quapnt01str = QVariant(QVariant::String);

  if (-1 == obj->GetFieldIndex("VALSOU")) {
    valsou = obj->GetFieldAsDouble("VALSOU");
    depth_value = valsou;
    sndfrm02str = SNDFRM02(obj, valsou);
  } else {
    if (geomType == wkbPolygon)
      least_depth = _DEPVAL01(obj, least_depth);

    if (least_depth == nan("")) {
      QVariant catobs(QVariant::Int);
      if (obj->GetFieldIndex("CATOBS") > 0)
        catobs = obj->GetFieldAsInteger("CATOBS");

      QVariant watlev(QVariant::Int);
      if (obj->GetFieldIndex("WATLEV") > 0)
        watlev = obj->GetFieldAsInteger("WATLEV");

      QVariant expsou(QVariant::Int);
      if (obj->GetFieldIndex("EXPSOU") > 0)
        expsou = obj->GetFieldAsInteger("EXPSOU");

      if (expsou.toInt() != 1) {
        if (catobs == 6)
          depth_value = 0.01;
        else if (watlev == 0) // default
          depth_value = -15.0;
        else {
          switch (watlev.toInt()) {
            case 5: depth_value =   0.0 ; break;
            case 3: depth_value =   0.01; break;
            case 4:
            case 1:
            case 2:
            default : depth_value = -15.0 ; break;
          }
        }
      }
    } else
      depth_value = least_depth;
  }

  udwhaz03str = _UDWHAZ03(obj, depth_value, lp, &b_promote);

  if (geomType == wkbPoint) {
    // Continuation A
    int      sounding    = FALSE;
    quapnt01str = CSQUAPNT01(obj);

    if (!udwhaz03str.isNull() && udwhaz03str.toString().length() > 0) {
      obstrn04str.append(udwhaz03str.toString());
      obstrn04str.append(quapnt01str.toString());

      goto end;
    }

    if (nan("") != valsou) {
      if (valsou <= 20.0) {
        int watlev = -9;
        if (obj->GetFieldIndex("WATLEV") > 0)
          obj->GetFieldAsInteger("WATLEV");

        if (!strncmp(obj->GetDefnRef()->GetName(), "UWTROC", 6)) {
          if (-9 == watlev) {  // default
            obstrn04str.append(";SY(DANGER51)");
            sounding = TRUE;
          } else {
            switch (watlev) {
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

          if (b_promote) {
            //  Move this UWTROC object to DisplayBase category
            lp->DISC = ChartDisplayCat::DISP_CAT_DISPLAYBASE;
          }
        } else { // OBSTRN
          if (-9 == watlev) { // default
            obstrn04str.append(";SY(DANGER01)");
            sounding = TRUE;
          } else {
            switch (watlev) {
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
      int watlev = -9;
      if (obj->GetFieldIndex("WATLEV") > 0)
        obj->GetFieldAsInteger("WATLEV");

      if (!strncmp(obj->GetDefnRef()->GetName(), "UWTROC", 6)) {
        if (watlev == -9)  // default
          obstrn04str.append(";SY(UWTROC04)");
        else {
          switch (watlev) {
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

        if (b_promote) {
          //  Move this UWTROC object to DisplayBase category
          lp->DISC = ChartDisplayCat::DISP_CAT_DISPLAYBASE;
        }
      } else { // OBSTRN
        if ( -9 == watlev) // default
          obstrn04str = ";SY(OBSTRN01)";
        else {
          switch (watlev) {
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
        int watlev = -9;
        if (obj->GetFieldIndex("WATLEV") > 0)
          watlev = obj->GetFieldAsInteger("WATLEV");

        if (watlev == -9)   // default
          obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");
        else {
          switch (watlev) {
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
              if (obj->GetFieldIndex("CATOBS") > 0)
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
  obstrn04str.append('\037');
  return obstrn04str;
}


/*
static void* CSDefault(void* obj) {
  return nullptr;
}

typedef ;



QMap<QString, void* (*)(void* params)> condTable[] = {
  {"CLRLIN01", nullptr } //CLRLIN01},
, {"DATCVR01", nullptr } //DATCVR01},
, {"DATCVR01", nullptr } //DATCVR01},
, {"DEPARE01", nullptr } //DEPARE01},
, {"DEPARE02", nullptr } //DEPARE01},                 // new in PLIB 3_3, opencpn defaults to DEPARE01
, {"DEPCNT02", nullptr } //DEPCNT02},
, {"DEPVAL01", nullptr } //DEPVAL01},
, {"LEGLIN02", nullptr } //LEGLIN02},
, {"LIGHTS05", nullptr } //LIGHTS05},                 // new in PLIB 3_3, replaces LIGHTS04
, {"LITDSN01", nullptr } //LITDSN01},
, {"OBSTRN04", OBSTRN04 }
, {"OWNSHP02", nullptr } //OWNSHP02},
, {"PASTRK01", nullptr } //PASTRK01},
, {"QUAPOS01", nullptr } //QUAPOS01},
, {"QUALIN01", nullptr } //QUALIN01},
, {"QUAPNT01", nullptr } //QUAPNT01},
, {"SLCONS03", nullptr } //SLCONS03},
, {"RESARE02", nullptr } //RESARE02},
, {"RESTRN01", nullptr } //RESTRN01},
//   {"RESCSP01", RESCSP01},
, {"SEABED01", nullptr } //SEABED01},
//   {"SNDFRM02", SNDFRM02},
, {"SOUNDG02", nullptr } //SOUNDG02},
, {"TOPMAR01", nullptr } //TOPMAR01},
, {"UDWHAZ03", nullptr } //UDWHAZ03},
, {"VESSEL01", nullptr } //VESSEL01},
, {"VRMEBL01", nullptr } //VRMEBL01},
, {"WRECKS02", nullptr } //WRECKS02},
, {"SOUNDG03", nullptr } //SOUNDG03},                   // special case for MPS
, {"########", nullptr }
};

*/
