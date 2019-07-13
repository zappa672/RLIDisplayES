// Based on OpenCPN s52cnsy.cpp

#include <QDebug>
#include <QMap>
#include <QVariant>

#include "s57condsymb.h"
#include "../common/rlistate.h"


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

QString _selSYcol(const QSet<int> cols, bool bsectr, double valnmr) {
  QString sym;

  if (!bsectr) {
    sym = ";SY(LITDEF11";   // default

    // max 1 color
    if (cols.size() == 1) {
      if (cols.contains(3))                               // strpbrk(buf, "\003")
        sym = ";SY(LIGHTS11";
      else if (cols.contains(4))                          // strpbrk(buf, "\004")
        sym = ";SY(LIGHTS12";
      else if (cols.intersects(QSet<int>{ 1, 6, 9 }))     // strpbrk(buf, "\001\006\011"
        sym = ";SY(LIGHTS13";
    } else {
      // max 2 color
      if (cols.size() == 1) {
        if (cols.contains(1) && cols.contains(3))         // strpbrk(buf, "\001") && strpbrk(buf, "\003")
          sym = ";SY(LIGHTS11";
        else if (cols.contains(1) && cols.contains(4))    // strpbrk(buf, "\001") && strpbrk(buf, "\004")
          sym = ";SY(LIGHTS12";
      }
    }
  // all-round fixed light symbolized as a circle, radius depends on color
  // This treatment is seen on SeeMyDenc by SevenCs
  // This may not be S-52 compliant....
  } else {
    // Another non-standard extension....
    // All round light circle diameter is scaled if the light has a reasonable VALNMR attribute
    int radius = 3;
    if (valnmr > 0) {
      if (valnmr < 7.0)
        radius = 3;
      else if (valnmr < 15.0)
        radius = 10;
      else if(valnmr < 30.0)
        radius = 15;
      else
        radius = 20;
    }

    // max 1 color
    if (cols.size() == 1) {
      if (cols.contains(3))                                           // strpbrk(buf, "\003")
        sym.sprintf(",LITRD, 2,0,360,%d,0", radius + 1);
      else if (cols.contains(4))                                      // strpbrk(buf, "\004"
        sym.sprintf(",LITGN, 2,0,360,%d,0", radius);
      else if (cols.intersects(QSet<int>{ 1, 6, 9 }))                 // strpbrk(buf, "\001\006\011") \011 = 7+1
        sym.sprintf(",LITYW, 2,0,360,%d,0", radius + 2);
      else if (cols.contains(11))                                     // strpbrk(buf, "\014") \014 = 7+4
        sym.sprintf(",CHMGD, 2,0,360,%d,0", radius + 3);
      else
        sym.sprintf(",CHMGD, 2,0,360,%d,0", radius + 5);              // default
    // or 2 color
    } else  if (cols.size() == 2) {
      if (cols.contains(1) && cols.contains(3))                       // strpbrk(buf, "\001") && strpbrk(buf, "\003")
        sym.sprintf(",LITRD, 2,0,360,%d,0", radius + 1);
      else if (cols.contains(1) && cols.contains(4))                  // strpbrk(buf, "\001") && strpbrk(buf, "\004")
        sym.sprintf(",LITGN, 2,0,360,%d,0", radius);
      else
        sym.sprintf(",CHMGD, 2,0,360,%d,0", radius + 5);              // default
    }
    else
      sym.sprintf(",CHMGD, 2,0,360,%d,0", radius + 5);

    if (sym.length())
      sym.prepend(";CA(OUTLW, 4");
  }

  return sym;
}


// Remarks: In S-57 the light characteristics are held as a series of attributes values. The
// mariner may wish to see a light description text string displayed on the
// screen similar to the string commonly found on a paper chart. This
// conditional procedure, reads the attribute values from the above list of
// attributes and composes a light description string which can be displayed.
// This procedure is provided as a C function which has as input, the above
// listed attribute values and as output, the light description.
static QString _LITDSN01(OGRFeature* obj) {
  // CATLIT, LITCHR, COLOUR, HEIGHT, LITCHR, SIGGRP, SIGPER, STATUS, VALNMR
  QString litdsn01;

  /*
   *   1: directional function     IP 30.1-3;  475.7;
   *   2: rear/upper light
   *   3: front/lower light
   *   4: leading light            IP 20.1-3;  475.6;
   *   5: aero light               IP 60;      476.1;
   *   6: air obstruction light    IP 61;      476.2;
   *   7: fog detector light       IP 62;      477;
   *   8: flood light              IP 63;      478.2;
   *   9: strip light              IP 64;      478.5;
   *   10: subsidiary light        IP 42;      471.8;
   *   11: spotlight
   *   12: front
   *   13: rear
   *   14: lower
   *   15: upper
   *   16: moire' effect           IP 31;      475.8;
   *   17: emergency
   *   18: bearing light                       478.1;
   *   19: horizontally disposed
   *   20: vertically disposed
   */

  // LITCHR
  QVariant litchr = getIntField(obj, "LITCHR");
  QString spost;
  bool b_grp2 = false;                      // 2 GRP attributes expected

  if(!litchr.isNull()) {
    switch (litchr.toInt()) {
      case 1:  litdsn01.append("F");                        break;  // fixed     IP 10.1;
      case 2:  litdsn01.append("Fl");                       break;  // flashing  IP 10.4;
      case 3:  litdsn01.append("LFl");                      break;  // long-flashing   IP 10.5;
      case 4:  litdsn01.append("Q");                        break;  // quick-flashing  IP 10.6;
      case 5:  litdsn01.append("VQ");                       break;  // very quick-flashing   IP 10.7;
      case 6:  litdsn01.append("UQ");                       break;  // ultra quick-flashing  IP 10.8;
      case 7:  litdsn01.append("Iso");                      break;  // isophased IP 10.3;
      case 8:  litdsn01.append("Occ");                      break;  // occulting IP 10.2;
      case 9:  litdsn01.append("IQ");                       break;  // interrupted quick-flashing  IP 10.6;
      case 10: litdsn01.append("IVQ");                      break;  // interrupted very quick-flashing   IP 10.7;
      case 11: litdsn01.append("IUQ");                      break;  // interrupted ultra quick-flashing  IP 10.8;
      case 12: litdsn01.append("Mo");                       break;  // morse     IP 10.9;
      case 13: litdsn01.append("F + Fl");   b_grp2 = true;  break;  // fixed/flash     IP 10.10;
      case 14: litdsn01.append("Fl + LFl"); b_grp2 = true;  break;  // flash/long-flash
      case 15: litdsn01.append("Occ + Fl"); b_grp2 = true;  break;  // occulting/flash
      case 16: litdsn01.append("F + LFl");  b_grp2 = true;  break;  // fixed/long-flash
      case 17: litdsn01.append("Al Occ");                   break;  // occulting alternating
      case 18: litdsn01.append("Al LFl");                   break;  // long-flash alternating
      case 19: litdsn01.append("Al Fl");                    break;  // flash alternating
      case 20: litdsn01.append("Al Grp");                   break;  // group alternating
      case 21: litdsn01.append("F");     spost = " (vert)"; break;  // 2 fixed (vertical)
      case 22: litdsn01.append("F");     spost = " (horz)"; break;  // 2 fixed (horizontal)
      case 23: litdsn01.append("F");     spost = " (vert)"; break;  // 3 fixed (vertical)
      case 24: litdsn01.append("F");     spost = " (horz)"; break;  // 3 fixed (horizontal)
      case 25: litdsn01.append("Q + LFl");  b_grp2 = true;  break;  // quick-flash plus long-flash
      case 26: litdsn01.append("VQ + LFl"); b_grp2 = true;  break;  // very quick-flash plus long-flash
      case 27: litdsn01.append("UQ + LFl"); b_grp2 = true;  break;  // ultra quick-flash plus long-flash
      case 28: litdsn01.append("Alt");                      break;  // alternating
      case 29: litdsn01.append("F + Alt");  b_grp2 = true;  break;  // fixed and alternating flashing
      default: break;
    }
  }

  int nfirst_grp = -1;
  if (b_grp2) {
    QString ret_new;
    nfirst_grp = litdsn01.indexOf(" ");
    if (nfirst_grp > 0) {
      ret_new = litdsn01.mid(0, nfirst_grp);
      ret_new.append("(?)");
      ret_new.append(litdsn01.mid(nfirst_grp));
      litdsn01 = ret_new;
      nfirst_grp += 1;
    }
  }

  // SIGGRP, (c)(c) ...
  QVariant grp_str = getStringField(obj, "SIGGRP");
  if (!grp_str.isNull()) {
    QString ss = grp_str.toString();

    if (b_grp2) {
      QStringList lst = ss.split(QRegExp("[()]"), QString::SkipEmptyParts);

      int n_tok = 0;
      while ((n_tok < lst.size()) && (n_tok < 2)) {
        QString s = lst[n_tok];
        if (s.length()) {
          if ((n_tok == 0) && (nfirst_grp > 0)) {
            litdsn01[nfirst_grp] = s[0];
          } else {
            if (s != "1") {
              litdsn01.append("(");
              litdsn01.append(s);
              litdsn01.append(")");
            }
          }

          n_tok++;
        }
      }
    } else {
      if (ss != "(1)")
        litdsn01.append(ss);
    }
  }

  // COLOUR,
  bool hasSectors = (obj->GetFieldIndex("SECTR1") >= 0);

  // Don't show for sectored lights since we are only showing one of the sectors.
  // no sectors
  if (!hasSectors) {
    QSet<int> cols = parseIntList(getStringField(obj, "COLOUR").toString());

    for (int col: cols)
      switch (col) {
        case 1:  litdsn01.append("W"); break;
        case 3:  litdsn01.append("R"); break;
        case 4:  litdsn01.append("G"); break;
        case 6:  litdsn01.append("Y"); break;
        default:  break;
      }
  }

  /*
    1: white      IP 11.1;    450.2-3;
    2: black
    3: red        IP 11.2;    450.2-3;
    4: green      IP 11.3;    450.2-3;
    5: blue       IP 11.4;    450.2-3;
    6: yellow     IP 11.6;    450.2-3;
    7: grey
    8: brown
    9: amber      IP 11.8;    450.2-3;
    10: violet    IP 11.5;    450.2-3;
    11: orange    IP 11.7;    450.2-3;
    12: magenta
    13: pink
  */

  // SIGPER, xx.xx
  QVariant sigper = getDoubleField(obj, "SIGPER");

  if (!sigper.isNull()) {
    QString s;
    if (fabs(std::round(sigper.toDouble()) - sigper.toDouble()) > 0.01)
      s.sprintf("%4.1fs", sigper.toDouble());
    else
      s.sprintf("%2.0fs", sigper.toDouble());

    s = s.trimmed();          // remove leading spaces
    s.prepend(" ");
    litdsn01.append(s);
  }

  // HEIGHT, xxx.x
  QVariant height = getDoubleField(obj, "HEIGHT");

  if (!height.isNull()) {
    QString s;
    switch(m_nDepthUnitDisplay) {
      case RLIDepthUnit::FEET:                       // feet
      case RLIDepthUnit::FATHOM:                       // fathoms
        s.sprintf("%3.0fft", height.toDouble() * 3 * 39.37 / 36);
        break;
      default:
        s.sprintf("%3.0fm", height.toDouble());
        break;
    }

    s = s.trimmed();          // remove leading spaces
    s.prepend(" ");
    litdsn01.append(s);
  }

  // VALNMR, xx.x
  QVariant valnmr = getDoubleField(obj, "VALNMR");
  if (!valnmr.isNull() && !hasSectors ) {
    QString s;
    s.sprintf("%2.0fNm", valnmr.toDouble());
    s = s.trimmed();          // remove leading spaces
    s.prepend(" ");
    litdsn01.append(s);
  }

  return litdsn01.append(spost);                     // add any final modifiers
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
  Q_UNUSED(obj);
  Q_UNUSED(least_depth);

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
static QString _UDWHAZ03(OGRFeature* obj, const QVariant& depth_value, LookUp* lp, OGRDataSource* ds) {
  QString udwhaz03str;

  bool danger = false;
  QVariant expsou = getIntField(obj, "EXPSOU");

  if (depth_value.isNull()) {
    if (!expsou.isNull() && expsou.toInt() != 1)
      danger = true;
  }

  QVector<OGRGeometry*> selfGeom;
  for (int i = 0; i < obj->GetGeomFieldCount(); i++)
    selfGeom << obj->GetGeomFieldRef(i);

  if (!danger && (expsou == 1 || depth_value <= CONST_SAFETY_DEPTH)) {
    // that intersect this point/line/area for OBSTRN04
    // that intersect this point/area      for WRECKS02

    // Collect associated features
    QVector<QString> lookupLayers { "DEPARE", "DRGARE" };
    bool found = false;
    for (const auto& lrName: lookupLayers) {
      OGRLayer* layer = ds->GetLayerByName(lrName.toStdString().data());
      if (layer == nullptr)
        continue;

      layer->ResetReading();

      OGRFeature* feat;
      while( (feat = layer->GetNextFeature()) != nullptr ) {
        for (int i = 0; i < feat->GetGeomFieldCount(); i++) {
          OGRGeometry* geom = feat->GetGeomFieldRef(i);
          for (OGRGeometry* sGeom: selfGeom) {
            if (!sGeom->Intersects(geom))
              continue;

            if (geom->getGeometryType() == wkbLineString) {
              QVariant drval2 = getDoubleField(feat, "DRVAL2");
              if (!drval2.isNull() && drval2.toDouble() < CONST_SAFETY_DEPTH) {
                danger = true;
                found = true;
              }
            } else {
              QVariant drval1 = getDoubleField(feat, "DRVAL1");
              if (!drval1.isNull() && drval1.toDouble() >= CONST_SAFETY_DEPTH && expsou.toInt() != 1) {
                danger = true;
                found = true;
              }
            }

            if (found)
              break;
          } // self feature geometries loop

          if (found)
            break;
        } // testing feature geometries loop

        OGRFeature::DestroyFeature(feat);
        if (found)
          break;
      } // -- layer features loop

      if (found)
        break;
    } // -- layers loop
  }

  if (danger) {
    QVariant watlev = getIntField(obj, "WATLEV");

    if (watlev.toInt() != 1 && watlev.toInt() != 2)
      udwhaz03str = ";SY(ISODGR51)";

    // Move this object to DisplayBase category
    lp->DISC = ChartDisplayCat::DISPLAYBASE;
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
// A line object may be composed of more than one spatial object.
//
// This procedure looks at each of the spatial
// objects, and symbolizes the line according to the positional accuracy.
QString CSQUALIN01(OGRFeature* obj) {
  QString qualin01;
  QVariant quapos = getIntField(obj, "QUAPOS");

  if (!quapos.isNull()) {
    if (2 <= quapos.toInt() && quapos.toInt() < 10)
      qualin01 = "LC(LOWACC21)";
  } else {
    if (!strncmp("COALNE", obj->GetDefnRef()->GetName(), 6)) {
      QVariant conrad = getIntField(obj, "CONRAD");

      if (!conrad.isNull()) {
        if (1 == conrad.toInt())
          qualin01 = "LS(SOLD,3,CHMGF);LS(SOLD,1,CSTLN)";
        else
          qualin01 = "LS(SOLD,1,CSTLN)";
      } else
        qualin01 = "LS(SOLD,1,CSTLN)";
    } else  //LNDARE
      qualin01 = "LS(SOLD,1,CSTLN)";
  }

  return qualin01;
}


// Remarks: Soundings differ from plain text because they have to be readable under all
// circumstances and their digits are placed according to special rules. This
// conditional symbology procedure accesses a set of carefully designed
// sounding symbols provided by the symbol library and composes them to
// sounding labels. It symbolizes swept depth and it also symbolizes for low
// reliability as indicated by attributes QUASOU and QUAPOS.
QString _SNDFRM02(OGRFeature* obj, double depth_value_in) {
  QString sndfrm02;
  std::string symbolPref;
  const char* symPrefData = symbolPref.data();

  QSet<int> tecsou = parseIntList(getStringField(obj, "TECSOU").toString());
  QSet<int> quasou = parseIntList(getStringField(obj, "QUASOU").toString());
  QSet<int> status = parseIntList(getStringField(obj, "STATUS").toString());
  QVariant quapos = getIntField(obj, "QUAPOS");

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
    symbolPref = "SOUNDS";
  else
    symbolPref = "SOUNDG";

  if (tecsou.contains(6))                               // strpbrk(tecsou, "\006")
    sndfrm02.append(QString().sprintf(";SY(%sB1)", symPrefData));


  //strpbrk(quasou, "\003\004\005\010\011") || strpbrk(status, "\022")
  if ( quasou.intersect(QSet<int> { 3, 4, 5, 8, 9 }).size() > 0
    || status.contains(18)
    || (2 <= quapos.toInt() && quapos.toInt() < 10) )
    sndfrm02.append(QString().sprintf(";SY(%sC2)", symPrefData));

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

      sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, static_cast<int>(abs(leading_digit))));
      if (fraction > 0)
        sndfrm02.append(QString().sprintf(";SY(%s5%1i)", symPrefData, fraction));

      // above sea level (negative)
      if (depth_value < 0.0)
        sndfrm02.append(QString().sprintf(";SY(%sA1)", symPrefData));

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
        sndfrm02.append(QString().sprintf(";SY(%s2%1i)", symPrefData, static_cast<int>(leading_digit/10)));

      double digit1 = floor(leading_digit / 10);
      int digit2 = static_cast<int>(floor(leading_digit - (digit1 * 10)));
      sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, digit2));

      if (!b_2digit)
        if (static_cast<int>(fraction) > 0)
          sndfrm02.append(QString().sprintf(";SY(%s5%1i)", symPrefData, static_cast<int>(fraction)));

      if (depth_value < 0.0)
        sndfrm02.append(QString().sprintf(";SY(%sA1)", symPrefData));

      return sndfrm02;
    }
  }

  // Continuation B
  if (fabs(depth_value) < 100.0) {
      leading_digit = fabs(leading_digit);

      double first_digit = floor(leading_digit / 10);
      double secnd_digit = floor(leading_digit - (first_digit * 10));

      if (depth_value < 0.0) {
        sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, static_cast<int>(first_digit)));
        sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, static_cast<int>(secnd_digit)));
        sndfrm02.append(QString().sprintf(";SY(%sA1)", symPrefData));
      } else {
        sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, static_cast<int>(first_digit)));
        sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, static_cast<int>(secnd_digit)));
      }

      return sndfrm02;
  }

  if (depth_value < 1000.0) {
    int first_digit = static_cast<int>(floor(leading_digit / 100));
    int secnd_digit = static_cast<int>(floor((leading_digit - (first_digit * 100)) / 10));
    int third_digit = static_cast<int>(floor(leading_digit - (first_digit * 100) - (secnd_digit * 10)));

    sndfrm02.append(QString().sprintf(";SY(%s2%1i)", symPrefData, first_digit));
    sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, secnd_digit));
    sndfrm02.append(QString().sprintf(";SY(%s0%1i)", symPrefData, third_digit));

    return sndfrm02;
  }

  if (depth_value < 10000.0) {      
    int first_digit = static_cast<int>(floor(leading_digit / 1000));
    int secnd_digit = static_cast<int>(floor((leading_digit - (first_digit * 1000)) / 100));
    int third_digit = static_cast<int>(floor((leading_digit - (first_digit * 1000) - (secnd_digit * 100)) / 10));
    int last_digit  = static_cast<int>(floor(leading_digit - (first_digit * 1000) - (secnd_digit * 100) - (third_digit * 10)));

    sndfrm02.append(QString().sprintf(";SY(%s2%1i)", symPrefData, first_digit));
    sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, secnd_digit));
    sndfrm02.append(QString().sprintf(";SY(%s0%1i)", symPrefData, third_digit));
    sndfrm02.append(QString().sprintf(";SY(%s4%1i)", symPrefData, last_digit));

    return sndfrm02;
  }

  // Continuation C
  int first_digit  = static_cast<int>(floor(leading_digit / 10000));
  int secnd_digit  = static_cast<int>(floor((leading_digit - (first_digit * 10000)) / 1000));
  int third_digit  = static_cast<int>(floor((leading_digit - (first_digit * 10000) - (secnd_digit * 1000)) / 100));
  int fourth_digit = static_cast<int>(floor((leading_digit - (first_digit * 10000) - (secnd_digit * 1000) - (third_digit * 100)) / 10));
  int last_digit   = static_cast<int>(floor(leading_digit - (first_digit * 10000) - (secnd_digit * 1000) - (third_digit * 100) - (fourth_digit * 10)));

  sndfrm02.append(QString().sprintf(";SY(%s3%1i)", symPrefData, first_digit));
  sndfrm02.append(QString().sprintf(";SY(%s2%1i)", symPrefData, secnd_digit));
  sndfrm02.append(QString().sprintf(";SY(%s1%1i)", symPrefData, third_digit));
  sndfrm02.append(QString().sprintf(";SY(%s0%1i)", symPrefData, fourth_digit));
  sndfrm02.append(QString().sprintf(";SY(%s4%1i)", symPrefData, last_digit));

  return sndfrm02;
}



// Remarks: See procedure RESTRN01
static QString _RESCSP01(OGRFeature* obj) {
  QString rescsp01;
  QSet<int> restrn = parseIntList(getStringField(obj, "RESTRN").toString());
  QString symb;

  if (!restrn.empty()) {
    // strpbrk(restrn, "\007\010\016")
    if (restrn.intersects(QSet<int> { 7, 8, 14 })) {
      // continuation A
      // strpbrk(restrn, "\001\002\003\004\005\006")
      if (restrn.intersects(QSet<int> { 1, 2, 3, 4, 5, 6 }))
        symb = ";SY(ENTRES61)";
      else {
        // strpbrk(restrn, "\011\012\013\014\015")
        if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(ENTRES71)";
        else
            symb = ";SY(ENTRES51)";
      }
    } else {
      // strpbrk(restrn, "\001\002")
      if (restrn.intersects(QSet<int> { 1, 2 })) {
        // continuation B
        // strpbrk(restrn, "\003\004\005\006")
        if (restrn.intersects(QSet<int> { 3, 4, 5, 6 }))
          symb = ";SY(ACHRES61)";
        else {
          // strpbrk(restrn, "\011\012\013\014\015")
          if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(ACHRES71)";
          else
            symb = ";SY(ACHRES51)";
        }
      } else {
        // strpbrk(restrn, "\003\004\005\006")
        if (restrn.intersects(QSet<int> { 3, 4, 5, 6 })) {
          // continuation C
          // strpbrk(restrn, "\011\012\013\014\015")
          if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(FSHRES71)";
          else
            symb = ";SY(FSHRES51)";
        } else {
          // strpbrk(restrn, "\011\012\013\014\015")
          if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(INFARE51)";
          else
            symb = ";SY(RSRDEF51)";
        }
      }
    }

    rescsp01.append(symb);
  }

  return rescsp01;
}






// Remarks: Obstructions or isolated underwater dangers of depths less than the safety
// contour which lie within the safe waters defined by the safety contour are
// to be presented by a specific isolated danger symbol and put in IMO
// category DISPLAYBASE (see (3), App.2, 1.3). This task is performed
// by the sub-procedure "UDWHAZ03" which is called by this symbology
// procedure. Objects of the class "under water rock" are handled by this
// routine as well to ensure a consistent symbolization of isolated dangers on
// the seabed.
static QString OBSTRN04(OGRFeature* obj, LookUp* lp, OGRDataSource* ds) {
  QString obstrn04str;
  QVariant udwhaz03str = QVariant(QVariant::String);

  QVariant catobs = getIntField(obj, "CATOBS");
  QVariant watlev = getIntField(obj, "WATLEV");
  QVariant expsou = getIntField(obj, "EXPSOU");
  QVariant valsou = getDoubleField(obj, "VALSOU");

  QVariant depth_value = QVariant(QVariant::Double);
  QVariant least_depth = QVariant(QVariant::Double);

  QString sndfrm02str;
  QVariant quapnt01str = QVariant(QVariant::String);

  if (!valsou.isNull()) {
    depth_value = valsou;
    sndfrm02str = _SNDFRM02(obj, valsou.toDouble());
  } else {
    if (lp->FTYP == ChartObjectType::AREA)
      least_depth = _DEPVAL01(obj, least_depth.toDouble());

    if (!least_depth.isNull())
      depth_value = least_depth;
    else
      if (expsou.toInt() != 1) {
        if (catobs == 6)
          depth_value = 0.01;
        else if (watlev.isNull() || watlev.toDouble() == 0.0) // default
          depth_value = -15.0;
        else if (watlev.toInt() == 5)
          depth_value = 0.0;
        else if (watlev.toInt() == 3)
          depth_value = 0.01;
        else
          depth_value = -15.0;
      }
  }

  udwhaz03str = _UDWHAZ03(obj, depth_value, lp, ds);
  quapnt01str = CSQUAPNT01(obj);


  if (lp->FTYP == ChartObjectType::POINT) {
    bool sounding = false;

    if (udwhaz03str.toString().length() > 0) {
      obstrn04str.append(udwhaz03str.toString());
      obstrn04str.append(quapnt01str.toString());
    } else {
      if (!valsou.isNull()) {
        if (valsou <= 20.0) {
          if (!strncmp(obj->GetDefnRef()->GetName(), "UWTROC", 6)) {
            if (watlev.isNull()) {  // default
              obstrn04str.append(";SY(DANGER51)");
              sounding = true;
            } else if (watlev.toInt() == 3) {
              obstrn04str.append(";SY(DANGER51)");
              sounding = true;
            } else if (watlev.toInt() == 4 ||watlev.toInt() == 5) {
              obstrn04str.append(";SY(UWTROC04)");
              sounding = false;
            } else {
              obstrn04str.append(";SY(DANGER51)");
              sounding = true;
            }
          } else { // OBSTRN
            if (watlev.isNull()) { // default
              obstrn04str.append(";SY(DANGER01)");
              sounding = true;
            } else if (watlev.toInt() == 1 || watlev.toInt() == 2) {
              obstrn04str.append(";SY(LNDARE01)");
              sounding = false;
            } else if (watlev.toInt() == 3) {
              obstrn04str.append(";SY(DANGER52)");
              sounding = true;
            } else if (watlev.toInt() == 4 || watlev.toInt() == 5) {
              obstrn04str.append(";SY(DANGER53)");
              sounding = true;
            } else {
              obstrn04str.append(";SY(DANGER51)");
              sounding = true;
            }
          }
        } else {  // valsou > 20.0
          obstrn04str.append(";SY(DANGER52)");
          sounding = true;
        }
      } else {  // NO valsou
        if (!strncmp(obj->GetDefnRef()->GetName(), "UWTROC", 6)) {
          if (watlev.isNull())  // default
            obstrn04str.append(";SY(UWTROC04)");
          else if (watlev.toInt() == 2)
            obstrn04str.append(";SY(LNDARE01)");
          else if (watlev.toInt() == 3)
            obstrn04str.append(";SY(UWTROC03)");
          else
            obstrn04str.append(";SY(UWTROC04)");
        } else { // OBSTRN
          if (watlev.isNull()) // default
            obstrn04str = ";SY(OBSTRN01)";
          else if (watlev.toInt() == 1)
            obstrn04str.append(";SY(OBSTRN11)");
          else if (watlev.toInt() == 2)
            obstrn04str.append(";SY(OBSTRN11)");
          else if (watlev.toInt() == 3)
            obstrn04str.append(";SY(OBSTRN01)");
          else if (watlev.toInt() == 4)
            obstrn04str.append(";SY(OBSTRN03)");
          else if (watlev.toInt() == 5)
            obstrn04str.append(";SY(OBSTRN03)");
          else
            obstrn04str.append(";SY(OBSTRN01)");
        }
      }

      if (sounding)
        obstrn04str.append(sndfrm02str);

      obstrn04str.append(quapnt01str.toString());
    }
  }

  if (lp->FTYP == ChartObjectType::LINE) {
    if (!quapnt01str.isNull() && quapnt01str.toString().length() > 1) {
      long quapos = quapnt01str.toLongLong();
      if (2 <= quapos && quapos < 10) {
        if (udwhaz03str.toString().length())
          obstrn04str.append(";LC(LOWACC41)");
        else
          obstrn04str.append(";LC(LOWACC31)");
      }
    } else if (udwhaz03str.toString().length()) {
      obstrn04str.append("LS(DOTT,2,CHBLK)");
    } else if (!valsou.isNull()) {
      if (valsou <= 20.0) {
        obstrn04str.append(";LS(DOTT,2,CHBLK)");
        obstrn04str.append(sndfrm02str);
      } else
        obstrn04str.append(";LS(DASH,2,CHBLK)");
    } else
      obstrn04str.append(";LS(DOTT,2,CHBLK)");
  }

  if (lp->FTYP == ChartObjectType::AREA) { // Area feature
    if (0 != udwhaz03str.toString().length()) {
      obstrn04str.append(";AC(DEPVS);AP(FOULAR01)");
      obstrn04str.append(";LS(DOTT,2,CHBLK)");
      obstrn04str.append(udwhaz03str.toString());
      obstrn04str.append(quapnt01str.toString());
    } else if (valsou.isNull()) {
      // BUG in CA49995B.000 if we get here because there is no color
      // beside NODATA (ie there is a hole in group 1 area!)
      //g_string_append(obstrn04, ";AC(UINFR)");

      if (valsou <= 20.0)
        obstrn04str.append(";LS(DOTT,2,CHBLK)");
      else
        obstrn04str.append(";LS(DASH,2,CHBLK)");

      obstrn04str.append(sndfrm02str);
      obstrn04str.append(quapnt01str.toString());
    } else {
      if (watlev.isNull())   // default
        obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");
      else if (watlev.toInt() == 1 || watlev.toInt() == 2)
        obstrn04str.append(";AC(CHBRN);LS(SOLD,2,CSTLN)");
      else if (watlev.toInt() == 4)
        obstrn04str.append(";AC(DEPIT);LS(DASH,2,CSTLN)");
      else if (watlev.toInt() == 5 || watlev.toInt() == 3) {
        QVariant catobs = getIntField(obj, "CATOBS");

        if (catobs.toInt() == 6)
          obstrn04str.append(";AC(DEPVS);AP(FOULAR01);LS(DOTT,2,CHBLK)");
        else
          obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");
      } else
        obstrn04str.append(";AC(DEPVS);LS(DOTT,2,CHBLK)");

      obstrn04str.append(quapnt01str.toString());
    }
  }

  return obstrn04str;
}


static QString CLRLIN01(LookUp* lp) {
  printf("s52csny : CLRLIN01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}


// Remarks: This conditional symbology procedure describes procedures for:
// - symbolizing the limit of ENC coverage;
// - symbolizing navigational purpose boundaries ("scale boundarie"); and
// - indicating overscale display.
//
// Note that the mandatory meta object CATQUA is symbolized by the look-up table.
//
// Because the methods adopted by an ECDIS to meet the IMO and IHO requirements
// listed on the next page will depend on the manufacturer's software, and cannot be
// described in terms of a flow chart in the same way as other conditional procedures,
// this procedure is in the form of written notes.
static QString DATCVR01() {
  QString datcvr01;

  //
  // 1- REQUIREMENT
  // (IMO/IHO specs. explenation)

  //
  // 2- ENC COVERAGE
  //
  // 2.1- Limit of ENC coverage
  // datcvr01 = g_string_new(";OP(3OD11060);LC(HODATA01)");

  datcvr01.append("LC(HODATA01)");

  // rule_str.Append("AC(DEPDW)");
  // FIXME: get cell extend

  // 2.2- No data areas
  // This can be done outside of CS (ie when clearing the screen in Mesa)
  // FIXME: ";OP(0---);AC(NODATA)"
  // FIXME: set geo to cover earth (!)

  //
  // 3- SCALE BOUNDARIES
  //
  // 3.1- Chart scale boundaties
  // FIXME;
  // g_string_append(datcvr01, ";LS(SOLD,1,CHGRD)");
  // -OR- LC(SCLBDYnn) (?)
  //
  // ;OP(3OS21030)

  // 3.2- Graphical index of navigational purpose
  // FIXME: draw extent of available SENC in DB

  //
  // 4- OVERSCALE
  //
  // FIXME: get meta date CSCL of DSPM field
  // FIXME: get object M_CSCL or CSCALE
  //
  // 4.1- Overscale indication
  // FIXME: compute, scale = [denominator of the compilation scale] /
  //                         [denominator of the display scale]
  // FIXME: draw overscale indication (ie TX("X%3.1f",scale))
  //
  // 4.2- Ovescale area at a chart scale boundary
  // FIXME: test if next chart is over scale (ie going from large scale chart
  //        to a small scale chart)
  // FIXME: draw AP(OVERSC01) on overscale part of display
  //g_string(";OP(3OS21030)");

  //
  // 4.3- Larger scale data available
  // FIXME: display indication of better scale available (?)

  return datcvr01;
}


// Remarks: An object of the class "depth area" is coloured and covered with fill patterns
// according to the mariners selections of shallow contour, safety contour and
// deep contour. This requires a decision making process based  on DRVAL1 and DRVAL2.
// Objects of the class "dredged area" are handled by this routine as well to
// ensure a consistent symbolization of areas that represent the surface of the
// seabed.
static QString DEPARE01(OGRFeature* obj, LookUp* lp) {
  QVariant drval1, drval2;

  // Determine the color based on mariner selections
  drval1 = getDoubleField(obj, "DRVAL1");
  drval2 = getDoubleField(obj, "DRVAL2");

  QString rule_str = "AC(DEPIT)";

  if (drval1.toDouble() >= 0.0 && drval2.toDouble() > 0.0)
    rule_str = "AC(DEPVS)";

  if (S52_MAR_TWO_SHADES) {
    if (drval1 >= CONST_SAFETY_DEPTH && drval2 >  CONST_SAFETY_DEPTH)
      rule_str = "AC(DEPDW)";
  } else {
    if (drval1 >= CONST_SHALLOW_DEPTH && drval2 > CONST_SHALLOW_DEPTH)
      rule_str = "AC(DEPMS)";

    if (drval1 >= CONST_SAFETY_DEPTH && drval2 > CONST_SAFETY_DEPTH)
      rule_str  = "AC(DEPMD)";

    if (drval1 >= CONST_DEEP_DEPTH && drval2 > CONST_DEEP_DEPTH)
      rule_str  = "AC(DEPDW)";
  }

  // If object is DRGARE....
  if (!strncmp(lp->OBCL.toStdString().data(), "DRGARE", 6)) {
    if (drval1.isNull())  //If DRVAL1 was not defined...
      rule_str = "AC(DEPMD)";

    rule_str.append(";AP(DRGARE01)");
    rule_str.append(";LS(DASH,1,CHGRF)");
  }
  // Todo Restrictions
  // char pval[30];
  // if(true == GetStringAttr(obj, "RESTRN", pval, 20)) {
  //   GString *rescsp01 = _RESCSP01(geo);
  //   if (NULL != rescsp01) {
  //     g_string_append(depare01, rescsp01->str);
  //     g_string_free(rescsp01, TRUE);
  //   }
  // }
  return rule_str;
}


// Remarks: A light is one of the most complex S-57 objects. Its presentation depends on
// whether it is a light on a floating or fixed platform, its range, it's colour and
// so on. This conditional symbology procedure derives the correct
// presentation from these parameters and also generates an area that shows the
// coverage of the light.
//
// Notes on light sectors:
// 1.) The radial leg-lines defining the light sectors are normally drawn to only 25mm
// from the light to avoid clutter (see Part C). However, the mariner should be able to
// select "full light-sector lines" and have the leg-lines extended to the nominal range
// of the light (VALMAR).
//
// 2.) Part C of this procedure symbolizes the sectors at the light itself. In addition,
// it should be possible, upon request, for the mariner to be capable of identifying
// the colour and sector limit lines of the sectors affecting the ship even if the light
// itself is off the display.
// [ed. last sentence in bold]
static QString LIGHTS06(OGRFeature* obj) {
  QString lights06;

  QVariant valnmr = getDoubleField(obj, "VALNMR");
  if (valnmr.isNull())
    valnmr = 9.0;

  QSet<int> catlit = parseIntList(getStringField(obj, "CATLIT").toString());
  QSet<int> litvis = parseIntList(getStringField(obj, "LITVIS").toString());

  QSet<int> colist = parseIntList(getStringField(obj, "COLOUR").toString());     // colour set

  QVariant sectr1       = getDoubleField(obj, "SECTR1");
  QVariant sectr2       = getDoubleField(obj, "SECTR2");

  bool     flare_at_45  = false;
  double   sweep        = 0.;
  bool     b_isflare    = false;

  QString orientstr;

  if (!catlit.empty()) {
    if (catlit.intersects(QSet<int>{ 8, 11 }))      //strpbrk(catlit, "\010\013")
      return lights06.append(";SY(LIGHTS82)");

    if (catlit.contains(9))                        //strpbrk(catlit, "\011")
      return lights06.append(";SY(LIGHTS81)");

    goto l06_end;
  }

  // Continuation A
  if (colist.isEmpty())
    // colist[0] = '\014';  // magenta (12)
    // colist[1] = '\000';
    colist = QSet<int> { 0, 12 }; // magenta (12)

  // This is not a sector light
  if (sectr1.isNull() || sectr2.isNull()) {
    QString ssym;

    if (valnmr < 10.0) {
      ssym = _selSYcol(colist, false, valnmr.toDouble());              // flare
      b_isflare = true;
      flare_at_45 = false;
    } else {
      ssym = _selSYcol(colist, true, valnmr.toDouble());              // all round light
      b_isflare = false;
    }

    //  Is the light a directional or moire?
    if (catlit.intersects(QSet<int>{ 1, 14 })) {          // strpbrk(catlit, "\001\016")
      if (orientstr.length()) {
        lights06.append(ssym);
        lights06.append(orientstr);
        lights06.append(";TE('%03.0lf deg','ORIENT',3,3,3,'15110',3,1,CHBLK,23)");
      } else
        lights06.append(";SY(QUESMRK1)");
    } else {
      lights06.append(ssym);

      if (b_isflare) {
        if (flare_at_45)
          lights06.append(",45)");
        else
          lights06.append(",135)");
      }
    }

    goto l06_end;
  }

  // Continuation B --sector light
  if (sectr1.isNull()) {
    sectr1 = 0.0;
    sectr2 = 0.0;
  } else
    sweep = (sectr1.toDouble() > sectr2.toDouble()) ? sectr2.toDouble()-sectr1.toDouble()+360
                                                    : sectr2.toDouble()-sectr1.toDouble();


  if (sweep<1.0 || sweep==360.0) {
    // handle all round light
    QString ssym = _selSYcol(colist, true, valnmr.toDouble());           // all round light
    lights06.append(ssym);

    goto l06_end;
  }

  // setup sector
  {
    double arc_radius = 20.;                // mm
    double sector_radius = 25.;

    // Another non-standard extension....
    // Sector light arc radius is scaled if the light has a reasonable VALNMR attribute
    if (valnmr.toDouble() > 0) {
      if (valnmr < 15.0)
        arc_radius = 10.;
      else if(valnmr < 30.0)
        arc_radius = 15.;
      else
        arc_radius = 20.;
    }

    QString sym = ";CA(OUTLW, 4";

    // max 1 color
    if (colist.size() == 1) {
      if (colist.contains(3))                               // strpbrk(colist, "\003")
        sym += ",LITRD, 2";
      else if (colist.contains(4))                          // strpbrk(colist, "\004")
        sym += ",LITGN, 2";
      else if (colist.intersects(QSet<int>{ 1, 6, 11 }))    // strpbrk(colist, "\001\006\013"
        sym += ",LITYW, 2";
      else
        sym += ",CHMGD, 2";                 // default is magenta
    } else if (colist.size() == 2) {
      if (colist.contains(1) && colist.contains(3))         // strpbrk(colist, "\001") && strpbrk(colist, "\003"
        sym += ",LITRD, 2";
      else if (colist.contains(1) && colist.contains(4))    // strpbrk(colist, "\001") && strpbrk(colist, "\004"
        sym += ",LITGN, 2";
      else
        sym += ",CHMGD, 2";                 // default is magenta
    } else
      sym += ",CHMGD, 2";                 // default is magenta


    // Obscured/faint sector?
    if (!litvis.isEmpty()) {
      if (litvis.intersects(QSet<int>{ 3, 7, 8 }))         // strpbrk(litvis, "\003\007\010")
        sym += ";CA(CHBLK, 4,CHBRN, 1";
    }

    if (sectr2.toDouble() <= sectr1.toDouble())
      sectr2 = sectr2.toDouble() + 360;

    // Sectors are defined from seaward
    if (sectr1.toDouble() > 180)
      sectr1 = sectr1.toDouble() - 180;
    else
      sectr1 = sectr1.toDouble() + 180;

    if(sectr2 > 180)
      sectr2 = sectr2.toDouble() - 180;
    else
      sectr2 = sectr2.toDouble() + 180;

    char arc_data[80];
    sprintf(arc_data, ",%5.1f, %5.1f, %5.1f, %5.1f", sectr1.toDouble(), sectr2.toDouble(), arc_radius, sector_radius);

    sym += arc_data;
    lights06 = sym;

    goto l06_end;
  }


l06_end:

  {
    // Only show Light in certain position once. Otherwise there will be clutter.
    static QString lastDescription;
    QString litdsn01 = _LITDSN01( obj );

    if (litdsn01.length()) {
      lastDescription = litdsn01;
      lights06.append( ";TX('" );
      lights06.append( litdsn01 );

      if (flare_at_45)
        lights06.append( "',3,3,3,'15110',2,-1,CHBLK,23)" );
      else
        lights06.append( "',3,2,3,'15110',2,0,CHBLK,23)" );
    }
  }

  return lights06;
}


// Remarks: An object of the class "depth contour" or "line depth area" is highlighted and must
// be shown under all circumstances if it matches the safety contour depth value
// entered by the mariner (see IMO PS 3.6). But, while the mariner is free to enter any
// safety contour depth value that he thinks is suitable for the safety of his ship, the
// SENC only contains a limited choice of depth contours. This symbology procedure
// determines whether a contour matches the selected safety contour. If the selected
// safety contour does not exist in the data, the procedure will default to the next deeper
// contour. The contour selected is highlighted as the safety contour and put in
// DISPLAYBASE. The procedure also identifies any line segment of the spatial
// component of the object that has a "QUAPOS" value indicating unreliable
// positioning, and symbolizes it with a double dashed line.
//
// Note: Depth contours are not normally labeled. The ECDIS may provide labels, on demand
// only as with other text, or provide the depth value on cursor picking
static QString DEPCNT02 (OGRFeature* obj, LookUp* lp, S52References* ref
                         , const QMap<QString, QVariant>& featAttrs, double next_safe_contour) {
  double depth_value = 0.0;
  bool safe = false;
  QString depcnt02;

  if (!strncmp(obj->GetDefnRef()->GetName(), "DEPARE", 6) && lp->FTYP == ChartObjectType::LINE) {
    QVariant drval1 = getDoubleField(obj, "DRVAL1");
    QVariant drval2 = getDoubleField(obj, "DRVAL2");

    if (drval1.toInt() <= CONST_SAFETY_DEPTH) {
      if (drval2.toInt() >= CONST_SAFETY_DEPTH)
        safe = true;
    } else {
      if (next_safe_contour >= 1e6)
        next_safe_contour = CONST_SAFETY_DEPTH;

      if (fabs(drval1.toDouble() - next_safe_contour) < 1e-4)
        safe = true;
    }

    depth_value = drval1.toDouble();
  } else {
    // continuation A (DEPCNT)
    QVariant valdco = getDoubleField(obj, "VALDCO");
    depth_value = valdco.toDouble();

    if (fabs(depth_value - CONST_SAFETY_DEPTH) < 1e-6)
      safe = true;   // this is useless !?!?
    else {
        if (next_safe_contour >= 1e6)
          next_safe_contour = CONST_SAFETY_DEPTH;

        if (fabs(valdco.toDouble() - next_safe_contour) < 1e-4)
          safe = true;
    }
  }

  // Continuation B
  QVariant quapos = getIntField(obj, "QUAPOS"); // QUAPOS is an E (Enumerated) type attribute

  if (0 != quapos.toInt()) {
    if (2 <= quapos.toInt() && quapos.toInt() < 10) {
      if (safe) {
        QString safeCntr = "LS(DASH,2,DEPSC)";
        LookUp lp = ref->findBestLookUp("SAFECD", featAttrs, LookUpTable::PLAIN_BOUNDARIES);
        if (lp.RCID >= 0)
          safeCntr = lp.INST.join(";");
        depcnt02 = ";" + safeCntr;
      } else
        depcnt02 = ";LS(DASH,1,DEPCN)";
    }
  } else {
    if (safe) {
      QString safeCntr = "LS(SOLD,2,DEPSC)";
      LookUp lp = ref->findBestLookUp("SAFECN", featAttrs, LookUpTable::PLAIN_BOUNDARIES);
      if (lp.RCID >= 0)
        safeCntr = lp.INST.join(";");
      depcnt02 = ";" + safeCntr;
    } else
      depcnt02 = ";LS(SOLD,1,DEPCN)";
  }

  if (safe) {
    // Move this object to DisplayBase category
    lp->DISC = ChartDisplayCat::DISPLAYBASE;
    //rzRules->obj->Scamin = 1e8;                 // effectively no SCAMIN
  }

  return depcnt02;
}



static QString DEPVAL01(LookUp* lp) {
  printf("s52csny : DEPVAL01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

static QString LEGLIN02(LookUp* lp) {
  printf("s52csny : LEGLIN02 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

static QString LITDSN01(LookUp* lp) {
  printf("s52csny : LITDSN01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

static QString OWNSHP02(LookUp* lp) {
  printf("s52csny : OWNSHP02 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

static QString PASTRK01(LookUp* lp) {
  printf("s52csny : PASTRK01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// to the spatial object, not the feature object.
// In OpenCPN implementation, QUAPOS of Point Objects has been converted to
// QUALTY attribute of object.
//
// This procedure passes the object to procedure QUALIN01 or QUAPNT01,
// which traces back to the spatial object, retrieves any QUAPOS attributes,
// and returns the appropriate symbolization to QUAPOS01.
static QString QUAPOS01(OGRFeature* obj, LookUp* lp) {
  if (lp->FTYP == ChartObjectType::LINE)
    return CSQUALIN01(obj);
  else
    return CSQUAPNT01(obj);
}

// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// A line object may be composed of more than one spatial object.
//
// This procedure looks at each of the spatial
// objects, and symbolizes the line according to the positional accuracy.
static QString QUALIN01(OGRFeature* obj) {
  return CSQUALIN01(obj);
}


// Remarks: The attribute QUAPOS, which identifies low positional accuracy, is attached
// only to the spatial component(s) of an object.
//
// This procedure retrieves any QUALTY (ne QUAPOS) attributes, and returns the
// appropriate symbols to the calling procedure.
static QString QUAPNT01(OGRFeature* obj) {
  return CSQUAPNT01(obj);
}


// Remarks: Shoreline construction objects which have a QUAPOS attribute on their
// spatial component indicating that their position is unreliable are symbolized
// by a special linestyle in the place of the varied linestyles normally used.
// Otherwise this procedure applies the normal symbolization.
static QString SLCONS03(OGRFeature* obj, LookUp* lp) {
  QString slcons03;

  QVariant quapos = getIntField(obj, "QUAPOS");
  if (lp->FTYP == ChartObjectType::POINT) {
    if (!quapos.isNull() && 2 <= quapos.toInt() && quapos.toInt() < 10)
      slcons03 ="SY(LOWACC01)";
  } else {
    // This instruction not found in PLIB 3.4, but seems to appear in later PLIB implementations
    // by commercial ECDIS providers, so.....
    if (lp->FTYP == ChartObjectType::AREA)
      slcons03 = "AP(CROSSX01);";

    // GEO_LINE and GEO_AREA are the same
    if (!quapos.isNull()) {
      if (2 <= quapos.toInt() && quapos.toInt() < 10)
        slcons03 ="LC(LOWACC01)";
    } else {
      QVariant condtn = getIntField(obj, "CONDTN");

      if (!condtn.isNull() && (1 == condtn.toInt() || 2 == condtn.toInt()))
        slcons03 = "LS(DASH,1,CSTLN)";
      else {
        QVariant catslc = getIntField(obj, "CATSLC");
        if (!catslc.isNull() && (6  == catslc.toInt() || 15 == catslc.toInt() || 16 == catslc.toInt()))      // Some sort of wharf
          slcons03 = "LS(SOLD,4,CSTLN)";
        else {
          QVariant watlev = getIntField(obj, "WATLEV");
          if (!watlev.isNull() && 2 == watlev.toInt())
            slcons03 = "LS(SOLD,2,CSTLN)";
          else
            if (!watlev.isNull() && (3 == watlev.toInt() || 4 == watlev.toInt()))
              slcons03 = "LS(DASH,2,CSTLN)";
            else
              slcons03 = "LS(SOLD,2,CSTLN)";  // default
        }
      }
    }
  }

  return slcons03;
}


// Remarks: A list-type attribute is used because an area of the object class RESARE may
// have more than one category (CATREA). For example an inshore traffic
// zone might also have fishing and anchoring prohibition and a prohibited
// area might also be a bird sanctuary or a mine field.
//
// This conditional procedure is set up to ensure that the categories of most
// importance to safe navigation are prominently symbolized, and to pass on
// all given information with minimum clutter. Only the most significant
// restriction is symbolized, and an indication of further limitations is given by
// a subscript "!" or "I". Further details are given under conditional
// symbology procedure RESTRN01
//
// Other object classes affected by attribute RESTRN are handled by
// conditional symbology procedure RESTRN01.
static QString RESARE02(OGRFeature* obj) {
  QString resare02;

  QSet<int> restrn = parseIntList(getStringField(obj, "RESTRN").toString());
  QSet<int> catrea = parseIntList(getStringField(obj, "CATREA").toString());

  QString symb;
  QString line;
  QString prio;


  if (restrn.isEmpty()) {
    // Continuation D
    if (catrea.isEmpty()) {
        //strpbrk(catrea, "\001\010\011\014\016\023\025\031")
        if (catrea.intersects(QSet<int> { 1, 8, 9, 16, 18, 19, 21, 25 } )) {
          // strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030")
          if (catrea.intersects(QSet<int> { 4, 5, 6, 7, 10, 18, 20, 22, 23, 24 } ))
            symb = ";SY(CTYARE71)";
          else
            symb = ";SY(CTYARE51)";
        } else {
          // strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030")
          if (catrea.intersects(QSet<int> { 4, 5, 6, 7, 10, 18, 20, 22, 23, 24 } ))
            symb = ";SY(INFARE51)";
          else
            symb = ";SY(RSRDEF51)";
        }
    } else
        symb = ";SY(RSRDEF51)";

    if (CONST_AREA_LOOKUP == LookUpTable::SYMBOLYZED_BOUNDARIES)
      line = ";LC(CTYARE51)";
    else
      line = ";LS(DASH,2,CHMGD)";

  // strpbrk(restrn, "\007\010\016")
  // entry restrictions
  } else if (restrn.intersects(QSet<int> { 7, 8, 14 })) {
      // Continuation A
      // strpbrk(restrn, "\001\002\003\004\005\006")
      if (restrn.intersects(QSet<int> { 1, 2, 3, 4, 5, 6 })) // anchoring, fishing, trawling
        symb = ";SY(ENTRES61)";
      else {
        // strpbrk(catrea, "\001\010\011\014\016\023\025\031")
        if (catrea.intersects(QSet<int> { 1, 8, 9, 12, 14, 17, 21, 25 }))
          symb = ";SY(ENTRES61)";
        else {
          // strpbrk(restrn, "\011\012\013\014\015")
          if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(ENTRES71)";
          else {
            // strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030")
            if (catrea.intersects(QSet<int> { 4, 5, 6, 7, 10, 18, 20, 22, 23, 24 }))
              symb = ";SY(ENTRES71)";
            else
              symb = ";SY(ENTRES51)";
          }
        }
      }

      if (CONST_AREA_LOOKUP == LookUpTable::SYMBOLYZED_BOUNDARIES)
          line = ";LC(RESARE51)";
      else
          line = ";LS(DASH,2,CHMGD)";

      prio = ";OP(6---)";  // display prio set to 6
  } else {
    // strpbrk(restrn, "\001\002")
    if (restrn.intersects( QSet<int> { 1, 2 } )) {                          // anchoring
      // Continuation B
      // strpbrk(restrn, "\003\004\005\006")
      if (restrn.intersects( QSet<int> { 3, 4, 5, 6 } ))
        symb = ";SY(ACHRES61)";
      else {
        // strpbrk(catrea, "\001\010\011\014\016\023\025\031")
        if (catrea.intersects( QSet<int> { 1, 8, 9, 12, 14, 17, 21, 25 } ))
          symb = ";SY(ACHRES61)";
        else {
          // strpbrk(restrn, "\011\012\013\014\015")
          if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(ACHRES71)";
          else {
            // strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030")
            if (catrea.intersects(QSet<int> { 4, 5, 6, 7, 10, 18, 20, 22, 23, 24 }))
              symb = ";SY(ACHRES71)";
            else
              symb = ";SY(RESTRN51)";
          }
        }
      }

      if (CONST_AREA_LOOKUP == LookUpTable::SYMBOLYZED_BOUNDARIES)
        line = ";LC(RESARE51)";     // could be ACHRES51 when _drawLC is implemented fully
      else
        line = ";LS(DASH,2,CHMGD)";

      prio = ";OP(6---)";  // display prio set to 6
    } else {
      // strpbrk(restrn, "\003\004\005\006")
      if (restrn.intersects( QSet<int> { 3, 4, 5, 6 } )) {              // fishing/trawling
        // Continuation C
        // strpbrk(catrea, "\001\010\011\014\016\023\025\031")
        if (catrea.intersects( QSet<int> { 1, 8, 9, 12, 14, 17, 21, 25 } ))
          symb = ";SY(FSHRES51)";
        else {
          // strpbrk(restrn, "\011\012\013\014\015")
          if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))
            symb = ";SY(FSHRES71)";
          else {
            // strpbrk(catrea, "\004\005\006\007\012\022\024\026\027\030")
            if (catrea.intersects(QSet<int> { 4, 5, 6, 7, 10, 18, 20, 22, 23, 24 }))
              symb = ";SY(FSHRES71)";
            else
              symb = ";SY(FSHRES51)";
          }
        }

        if (CONST_AREA_LOOKUP == LookUpTable::SYMBOLYZED_BOUNDARIES)
          line = ";LC(FSHRES51)";
        else
          line = ";LS(DASH,2,CHMGD)";

        prio = ";OP(6---)";  // display prio set to 6
      } else {
        // strpbrk(restrn, "\011\012\013\014\015")
        if (restrn.intersects(QSet<int> { 9, 10, 11, 12, 13 }))        // diving, dredging, waking...
            symb = ";SY(INFARE51)";
        else
            symb = ";SY(RSRDEF51)";

        if (CONST_AREA_LOOKUP == LookUpTable::SYMBOLYZED_BOUNDARIES)
            line = ";LC(CTYARE51)";
        else
            line = ";LS(DASH,2,CHMGD)";
      }

      //  Todo more for s57 3.1  Look at caris catalog ATTR::RESARE
    }
  }


  // create command word
  if (prio.length())
      resare02.append(prio);
  resare02.append(line);
  resare02.append(symb);

  return resare02;
}


// Remarks: Objects subject to RESTRN01 are actually symbolised in sub-process
// RESCSP01, since the latter can also be accessed from other conditional
// symbology procedures. RESTRN01 merely acts as a "signpost" for
// RESCSP01.
//
// Object class RESARE is symbolised for the effect of attribute RESTRN in a separate
// conditional symbology procedure called RESARE02.
//
// Since many of the areas concerned cover shipping channels, the number of symbols used
// is minimised to reduce clutter. To do this, values of RESTRN are ranked for significance
// as follows:
// "Traffic Restriction" values of RESTRN:
// (1) RESTRN 7,8: entry prohibited or restricted
//     RESTRN 14: IMO designated "area to be avoided" part of a TSS
// (2) RESTRN 1,2: anchoring prohibited or restricted
// (3) RESTRN 3,4,5,6: fishing or trawling prohibited or restricted
// (4) "Other Restriction" values of RESTRN are:
//     RESTRN 9, 10: dredging prohibited or restricted,
//     RESTRN 11,12: diving prohibited or restricted,
//     RESTRN 13   : no wake area.
static QString RESTRN01 (OGRFeature* obj) {
  return _RESCSP01(obj);
}


static QString SEABED01(LookUp* lp) {
  printf("s52csny : SEABED01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}


// Remarks: In S-57 soundings are elements of sounding arrays rather than individual
// objects. Thus the conditional symbology methodology must examine each
// sounding of a sounding array one by one. To symbolize the depth values it
// calls the procedure SNDFRM02 which in turn translates the depth values
// into a set of symbols to be shown at the soundings position.
static QString SOUNDG02() {
  // Shortcut.  This CS method causes a branch to an S52plib method
  // which splits multi-point soundings into separate point objects,
  // and then calls CS(SOUNDG03) on successive points below.
  return "MP();";
}


// Remarks: Topmark objects are to be symbolized through consideration of their
// platforms e.g. a buoy. Therefore this conditional symbology procedure
// searches for platforms by looking for other objects that are located at the
// same position.. Based on the finding whether the platform is rigid or
// floating, the respective upright or sloping symbol is selected and presented
// at the objects location. Buoyf symbols and topmark symbols have been
// carefully designed to fit to each other when combined at the same position.
// The result is a composed symbol that looks like the traditional symbols the
// mariner is used to.
static QString TOPMAR01 (OGRFeature* obj, const QSet<int>& floatingATONArray) {
  QString sy;
  QVariant topshp = getIntField(obj, "TOPSHP");

  if (topshp.isNull())
    sy = ";SY(QUESMRK1)";
  else {
    /*
    if (TRUE == _atPtPos(obj, GetChartFloatingATONArray( rzRules ), false))
      floating = TRUE;
    else
      // FIXME: this test is wierd since it doesn't affect 'floating'
      if (TRUE == _atPtPos(obj, GetChartRigidATONArray( rzRules ), false))
        floating = FALSE;
    */
    int RCID = obj->GetFieldAsInteger("RCID");

    if (floatingATONArray.contains(RCID)) {
      // floating platform
      switch (topshp.toInt()) {
          case 1 : sy = ";SY(TOPMAR02)"; break;
          case 2 : sy = ";SY(TOPMAR04)"; break;
          case 3 : sy = ";SY(TOPMAR10)"; break;
          case 4 : sy = ";SY(TOPMAR12)"; break;

          case 5 : sy = ";SY(TOPMAR13)"; break;
          case 6 : sy = ";SY(TOPMAR14)"; break;
          case 7 : sy = ";SY(TOPMAR65)"; break;
          case 8 : sy = ";SY(TOPMAR17)"; break;

          case 9 : sy = ";SY(TOPMAR16)"; break;
          case 10: sy = ";SY(TOPMAR08)"; break;
          case 11: sy = ";SY(TOPMAR07)"; break;
          case 12: sy = ";SY(TOPMAR14)"; break;

          case 13: sy = ";SY(TOPMAR05)"; break;
          case 14: sy = ";SY(TOPMAR06)"; break;
          case 17: sy = ";SY(TMARDEF2)"; break;
          case 18: sy = ";SY(TOPMAR10)"; break;

          case 19: sy = ";SY(TOPMAR13)"; break;
          case 20: sy = ";SY(TOPMAR14)"; break;
          case 21: sy = ";SY(TOPMAR13)"; break;
          case 22: sy = ";SY(TOPMAR14)"; break;

          case 23: sy = ";SY(TOPMAR14)"; break;
          case 24: sy = ";SY(TOPMAR02)"; break;
          case 25: sy = ";SY(TOPMAR04)"; break;
          case 26: sy = ";SY(TOPMAR10)"; break;

          case 27: sy = ";SY(TOPMAR17)"; break;
          case 28: sy = ";SY(TOPMAR18)"; break;
          case 29: sy = ";SY(TOPMAR02)"; break;
          case 30: sy = ";SY(TOPMAR17)"; break;

          case 31: sy = ";SY(TOPMAR14)"; break;
          case 32: sy = ";SY(TOPMAR10)"; break;
          case 33: sy = ";SY(TMARDEF2)"; break;
          default: sy = ";SY(TMARDEF2)"; break;
      }
    } else {
      // not a floating platform
      switch (topshp.toInt()) {
          case 1 : sy = ";SY(TOPMAR22)"; break;
          case 2 : sy = ";SY(TOPMAR24)"; break;
          case 3 : sy = ";SY(TOPMAR30)"; break;
          case 4 : sy = ";SY(TOPMAR32)"; break;

          case 5 : sy = ";SY(TOPMAR33)"; break;
          case 6 : sy = ";SY(TOPMAR34)"; break;
          case 7 : sy = ";SY(TOPMAR85)"; break;
          case 8 : sy = ";SY(TOPMAR86)"; break;

          case 9 : sy = ";SY(TOPMAR36)"; break;
          case 10: sy = ";SY(TOPMAR28)"; break;
          case 11: sy = ";SY(TOPMAR27)"; break;
          case 12: sy = ";SY(TOPMAR14)"; break;

          case 13: sy = ";SY(TOPMAR25)"; break;
          case 14: sy = ";SY(TOPMAR26)"; break;
          case 15: sy = ";SY(TOPMAR88)"; break;
          case 16: sy = ";SY(TOPMAR87)"; break;

          case 17: sy = ";SY(TMARDEF1)"; break;
          case 18: sy = ";SY(TOPMAR30)"; break;
          case 19: sy = ";SY(TOPMAR33)"; break;
          case 20: sy = ";SY(TOPMAR34)"; break;

          case 21: sy = ";SY(TOPMAR33)"; break;
          case 22: sy = ";SY(TOPMAR34)"; break;
          case 23: sy = ";SY(TOPMAR34)"; break;
          case 24: sy = ";SY(TOPMAR22)"; break;

          case 25: sy = ";SY(TOPMAR24)"; break;
          case 26: sy = ";SY(TOPMAR30)"; break;
          case 27: sy = ";SY(TOPMAR86)"; break;
          case 28: sy = ";SY(TOPMAR89)"; break;

          case 29: sy = ";SY(TOPMAR22)"; break;
          case 30: sy = ";SY(TOPMAR86)"; break;
          case 31: sy = ";SY(TOPMAR14)"; break;
          case 32: sy = ";SY(TOPMAR30)"; break;
          case 33: sy = ";SY(TMARDEF1)"; break;
          default: sy = ";SY(TMARDEF1)"; break;
      }
    }
  }

  return sy;
}



static QString UDWHAZ03(LookUp* lp) {
  printf("s52csny : UDWHAZ03 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

static QString VESSEL01(LookUp* lp) {
  printf("s52csny : VESSEL01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}

static QString VRMEBL01(LookUp* lp) {
  printf("s52csny : VRMEBL01 ERROR no conditional symbology for: %s\n", lp->OBCL.toStdString().data());
  return QString();
}


// Remarks: Wrecks of depths less than the safety contour which lie within the safe waters
// defined by the safety contour are to be presented by a specific isolated
// danger symbol and put in IMO category DISPLAYBASE (see (3), App.2,
// 1.3). This task is performed by the sub-procedure "UDWHAZ03" which is
// called by this symbology procedure.
static QString WRECKS02 (OGRFeature* obj, LookUp* lp, OGRDataSource* ds) {
  QString wrecks02str;
  QString sndfrm02str;
  QString udwhaz03str;
  QString quapnt01str;
  QVariant least_depth = QVariant(QVariant::Double);
  double depth_value = 0.0;
  QVariant valsou      = getDoubleField(obj, "VALSOU");
  QVariant watlev      = getIntField(obj, "WATLEV");
  QVariant catwrk      = getIntField(obj, "CATWRK");

  int iquasou = -9;
  QSet<int> quasou = parseIntList(getStringField(obj, "QUASOU").toString());

  if (!valsou.isNull()) {
    depth_value = valsou.toDouble();
    sndfrm02str = _SNDFRM02(obj, depth_value);
  } else {
    if (lp->FTYP == ChartObjectType::AREA)
      least_depth = _DEPVAL01(obj, least_depth.toDouble());

    if (!least_depth.isNull())
      depth_value = least_depth.toDouble();
    else {
      if (!catwrk.isNull()) {
        switch (catwrk.toInt()) {
          case 1: depth_value =  20.0; break;       // safe
          case 2: depth_value =   0.0; break;       // dangerous
          case 4:
          case 5: depth_value = -15.0; break;
        }
      } else {
        if (watlev.isNull()) // default
          depth_value = -15.0;
        else
          switch (watlev.toInt()) {
            case 1:
            case 2: depth_value = -15.0 ; break;
            case 3: depth_value =   0.01; break;
            case 4: depth_value = -15.0 ; break;
            case 5: depth_value =   0.0 ; break;
            case 6: depth_value = -15.0 ; break;
          }
      }
    }

  }

  // ?????????????????????????????????????????????????????????????
  // Fixes FS 165   XXX where it is?
  // 7 is 'least depth unknown, safe clearance at value shown'
  if (quasou.isEmpty() || !quasou.contains(7)) // quasouchar[0] == 0 || NULL == strpbrk(quasouchar, "\07")
    udwhaz03str = _UDWHAZ03(obj, depth_value, lp, ds);
  else
    iquasou = 7;

  quapnt01str = CSQUAPNT01(obj);

  if (lp->FTYP == ChartObjectType::POINT) {
    if (udwhaz03str.length() > 0) {
      wrecks02str = udwhaz03str;
      wrecks02str.append(quapnt01str);
    } else {
      // Continuation A (POINT_T)
      if (!valsou.isNull()) {
        ///////////////////////////////////////////
        //    DSR New logic here, FIXME check s52 specs
        /*
        if (valsou <= 20.0) {
          wrecks02str = wxString(";SY(DANGER51)");
          if (NULL != sndfrm02str)
            wrecks02str.Append(sndfrm02str);
        } else
          wrecks02str = wxString(";SY(DANGER52)"); */

        if ((valsou.toDouble() < CONST_SAFETY_DEPTH)/* || (2 == catwrk)*/)    // maybe redundant, seems like wrecks with valsou < 20
                                                                              // are always coded as "dangerous wrecks"
                                                                              // Excluding (2 == catwrk) matches Caris logic
          wrecks02str = ";SY(DANGER51)";
        else
          wrecks02str = ";SY(DANGER52)";

        wrecks02str.append(";TX('Wk',2,1,2,'15110',1,0,CHBLK,21)");

        if (iquasou == 7) //Fixes FS 165
          wrecks02str.append(";SY(WRECKS07)");

        wrecks02str.append(sndfrm02str);       // always show valsou depth
        ///////////////////////////////////////////

        wrecks02str.append(udwhaz03str);
        wrecks02str.append(quapnt01str);
      } else {
       QString sym;

       if (!catwrk.isNull() && !watlev.isNull()) {
          if (1 == catwrk.toInt() && 3 == watlev.toInt())
            sym = ";SY(WRECKS04)";
          else {
            if (2 == catwrk.toInt() && 3 == watlev.toInt())
              sym = ";SY(WRECKS05)";
            else {
              if (4 == catwrk.toInt() || 5 == catwrk.toInt())
                sym = ";SY(WRECKS01)";
              else {
                if ((QSet<int> {1, 2, 4, 5}).contains(watlev.toInt()))
                  sym = ";SY(WRECKS01)";
                else
                  sym = ";SY(WRECKS05)"; // default
              }
            }
          }
        }

        wrecks02str = sym;
        wrecks02str.append(quapnt01str);
      }
    }
  } else {
        // Continuation B (AREAS_T)
        QVariant quapos = getIntField(obj, "QUAPOS");
        QString line;

        if (2 <= quapos.toInt() && quapos.toInt() < 10)
          line = ";LC(LOWACC41)";
        else {
          if (udwhaz03str.length() != 0)
            line = ";LS(DOTT,2,CHBLK)";
          else {
            if (!valsou.isNull()){
              if (valsou <= 20)
                line = ";LS(DOTT,2,CHBLK)";
              else
                line = ";LS(DASH,2,CHBLK)";
            } else {
              if (watlev.isNull())
                line = ";LS(DOTT,2,CSTLN)";
              else {
                switch (watlev.toInt()) {
                  case 1:
                  case 2: line = ";LS(SOLD,2,CSTLN)"; break;
                  case 4: line = ";LS(DASH,2,CSTLN)"; break;
                  case 3:
                  case 5:
                  default : line = ";LS(DOTT,2,CSTLN)"; break;
                }
              }
            }
          }
        }

        wrecks02str = QString(line);

        if (!valsou.isNull()) {
          if (valsou <= 20) {
            wrecks02str.append(udwhaz03str);
            wrecks02str.append(quapnt01str);
            wrecks02str.append(sndfrm02str);
          } else {
            // NOTE: ??? same as above ???
            wrecks02str.append(udwhaz03str);
            wrecks02str.append(quapnt01str);
          }
        } else {
          QString ac;

          if (watlev.isNull())
            ac = ";AC(DEPVS)";
          else
            switch (watlev.toInt()) {
              case 1:
              case 2: ac = ";AC(CHBRN)"; break;
              case 4: ac = ";AC(DEPIT)"; break;
              case 5:
              case 3:
              default : ac = ";AC(DEPVS)"; break;
            }

          wrecks02str.append(ac);

          wrecks02str.append(udwhaz03str);
          wrecks02str.append(quapnt01str);
        }
    }

  return wrecks02str;
}


// Remarks:  SOUNDG03 is a private conditional symbology,
// called to render individual points of a multi-point sounding set.
static QString SOUNDG03(OGRFeature* obj, OGRGeometry* geom) {
  if (geom->getGeometryType() != wkbPoint)
    return "";

  OGRPoint* p = static_cast<OGRPoint*>(geom);
  return _SNDFRM02(obj, p->getZ());
}




QString expandCondSymb( QString cs
                      , OGRFeature* obj
                      , OGRGeometry* geom
                      , LookUp* lp
                      , OGRDataSource* ds
                      , S52References* ref
                      , const QMap<QString, QVariant>& featAttrs
                      , double next_safe_cnt
                      , const QSet<int>& floatingATONArray
                      , const QSet<int>& rigidATONArray) {
  Q_UNUSED(rigidATONArray);

  if (cs == "OBSTRN04")
    return OBSTRN04(obj, lp, ds);
  if (cs == "CLRLIN01")
    return CLRLIN01(lp);
  if (cs == "DATCVR01")
    return DATCVR01();
  if (cs == "DEPARE01")
    return DEPARE01(obj, lp);
  if (cs == "DEPARE02")
    return DEPARE01(obj, lp);                 // new in PLIB 3_3, opencpn defaults to DEPARE01
  if (cs == "LIGHTS05")
    return LIGHTS06(obj);       // new in PLIB 3_3, replaces LIGHTS04, 4_0 replaces LIGHTS05
  if (cs == "DEPCNT02")
    return DEPCNT02(obj, lp, ref, featAttrs, next_safe_cnt);
  if (cs == "DEPVAL01")
    return DEPVAL01(lp);
  if (cs == "LEGLIN02")
    return LEGLIN02(lp);
  if (cs == "LITDSN01")
    return LITDSN01(lp);
  if (cs == "OWNSHP02")
    return OWNSHP02(lp);
  if (cs == "PASTRK01")
    return PASTRK01(lp);
  if (cs == "QUAPOS01")
    return QUAPOS01(obj, lp);
  if (cs == "QUALIN01")
    return QUALIN01(obj);
  if (cs == "QUAPNT01")
    return QUAPNT01(obj);
  if (cs == "SLCONS03")
    return SLCONS03(obj, lp);
  if (cs == "RESARE02")
    return RESARE02(obj);
  if (cs == "RESTRN01")
    return RESTRN01(obj);
  if (cs == "SEABED01")
    return SEABED01(lp);
  if (cs == "SOUNDG02")
    return SOUNDG02();
  if (cs == "TOPMAR01")
    return TOPMAR01(obj, floatingATONArray);
  if (cs == "UDWHAZ03")
    return UDWHAZ03(lp);
  if (cs == "VESSEL01")
    return VESSEL01(lp);
  if (cs == "VRMEBL01")
    return VRMEBL01(lp);
  if (cs == "WRECKS02")
    return WRECKS02(obj, lp, ds);
  if (cs == "SOUNDG03")
    return SOUNDG03(obj, geom);

  return "";
}
