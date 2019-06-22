#include "s52references.h"

#include <QFile>
#include <QDebug>
#include <QXmlStreamReader>


S52References::S52References(QString fileName) {
  QFile file(fileName);
  file.open(QFile::ReadOnly);
  QXmlStreamReader* xml = new QXmlStreamReader(&file);

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "color-tables")  readColorTables(xml);
      if (xml->name() == "lookups")       readLookUps(xml);
      if (xml->name() == "line-styles")   readLineStyles(xml);
      if (xml->name() == "patterns")      readPatterns(xml);
      if (xml->name() == "symbols")       readSymbols(xml);
      break;

    default:
      break;
    }
  }

  file.close();

  fillColorTables();
  //print();
}


LookUp* S52References::findBestLookUp(const QString& name, OGRFeature* obj, LookUpTable tbl, bool bStrict) {
  Q_UNUSED(obj);
  Q_UNUSED(bStrict);

  //obj->GetFieldAsString();

  if ( !lookups.contains(tbl)
    || !lookups[tbl].contains(name))
    return nullptr;

  QVector<LookUp*> lups = lookups[tbl][name];
  LookUp* best = lookups.size() > 0 ? lups[0] : nullptr;

  /*for (LookUp* lup: lookups) {

  }*/

  return best;
}


/*
LUPrec *s52plib::FindBestLUP( wxArrayOfLUPrec *LUPArray, unsigned int startIndex, unsigned int count, S57Obj *pObj, bool bStrict )
{
    int nATTMatch = 0;
    int countATT = 0;
    bool bmatch_found = false;

    if( pObj->att_array == NULL )
        goto check_LUP;       // object has no attributes to compare, so return "best" LUP

    for( unsigned int i = 0; i < count; ++i ) {
        LUPrec *LUPCandidate = LUPArray->Item( startIndex + i );

        if( !LUPCandidate->ATTArray.size() )
            continue;        // this LUP has no attributes coded

        countATT = 0;
        char *currATT = pObj->att_array;
        int attIdx = 0;

        for( unsigned int iLUPAtt = 0; iLUPAtt < LUPCandidate->ATTArray.size(); iLUPAtt++ ) {

            // Get the LUP attribute name
            char *slatc = LUPCandidate->ATTArray[iLUPAtt];

            if( slatc && (strlen(slatc) < 6) )
                goto next_LUP_Attr;         // LUP attribute value not UTF8 convertible (never seen in PLIB 3.x)

            if( slatc ){
                char *slatv = slatc + 6;
                while( attIdx < pObj->n_attr ) {
                    if( 0 == strncmp( slatc, currATT, 6 ) ) {
                        //OK we have an attribute name match


                        bool attValMatch = false;

                        // special case (i)
                        if( !strncmp( slatv, " ", 1 ) ) {        // any object value will match wild card (S52 para 8.3.3.4)
                            ++countATT;
                            goto next_LUP_Attr;
                        }

                        // special case (ii)
                        //TODO  Find an ENC with "UNKNOWN" DRVAL1 or DRVAL2 and debug this code
                        if( !strncmp( slatv, "?", 1) ){          // if LUP attribute value is "undefined"

                        //  Match if the object does NOT contain this attribute
                            goto next_LUP_Attr;
                        }


                        //checking against object attribute value
                        S57attVal *v = ( pObj->attVal->Item( attIdx ) );

                        switch( v->valType ){
                            case OGR_INT: // S57 attribute type 'E' enumerated, 'I' integer
                            {
                                int LUP_att_val = atoi( slatv );
                                if( LUP_att_val == *(int*) ( v->value ) )
                                    attValMatch = true;
                                break;
                            }

                            case OGR_INT_LST: // S57 attribute type 'L' list: comma separated integer
                            {
                                int a;
                                char ss[41];
                                strncpy( ss, slatv, 39 );
                                ss[40] = '\0';
                                char *s = &ss[0];

                                int *b = (int*) v->value;
                                sscanf( s, "%d", &a );

                                while( *s != '\0' ) {
                                    if( a == *b ) {
                                        sscanf( ++s, "%d", &a );
                                        b++;
                                        attValMatch = true;

                                    } else
                                        attValMatch = false;
                                }
                                break;
                            }
                            case OGR_REAL: // S57 attribute type'F' float
                            {
                                double obj_val = *(double*) ( v->value );
                                float att_val = atof( slatv );
                                if( fabs( obj_val - att_val ) < 1e-6 )
                                    if( obj_val == att_val  )
                                        attValMatch = true;
                                break;
                            }

                            case OGR_STR: // S57 attribute type'A' code string, 'S' free text
                            {
                                //    Strings must be exact match
                                //    n.b. OGR_STR is used for S-57 attribute type 'L', comma-separated list

                                //wxString cs( (char *) v->value, wxConvUTF8 ); // Attribute from object
                                //if( LATTC.Mid( 6 ) == cs )
                                if( !strcmp((char *) v->value, slatv))
                                    attValMatch = true;
                                break;
                            }

                            default:
                                break;
                        } //switch

                        // value match
                        if( attValMatch )
                            ++countATT;

                        goto next_LUP_Attr;
                    } // if attribute name match

                    //  Advance to the next S57obj attribute
                    currATT += 6;
                    ++attIdx;

                } //while
            } //if

next_LUP_Attr:

            currATT = pObj->att_array; // restart the object attribute list
            attIdx = 0;
        } // for iLUPAtt

        //      Create a "match score", defined as fraction of candidate LUP attributes
        //      actually matched by feature.
        //      Used later for resolving "ties"

        int nattr_matching_on_candidate = countATT;
        int nattrs_on_candidate = LUPCandidate->ATTArray.size();
        double candidate_score = ( 1. * nattr_matching_on_candidate )
        / ( 1. * nattrs_on_candidate );

        //       According to S52 specs, match must be perfect,
        //         and the first 100% match is selected
        if( candidate_score == 1.0 ) {
            LUP = LUPCandidate;
            bmatch_found = true;
            break; // selects the first 100% match
        }

    } //for loop


check_LUP:
//  In strict mode, we require at least one attribute to match exactly

    if( bStrict ) {
        if( nATTMatch == 0 ) // nothing matched
            LUP = NULL;
    } else {
        //      If no match found, return the first LUP in the list which has no attributes
        if( !bmatch_found ) {
            for( unsigned int j = 0; j < count; ++j ) {
                LUPrec *LUPtmp = NULL;

                LUPtmp = LUPArray->Item( startIndex + j );
                if( !LUPtmp->ATTArray.size() ) {
                    return LUPtmp;
                }
            }
        }
    }

    return LUP;
}
*/


void S52References::fillColorTables() {
  uint current_index = 0;
  QList<QString> table_names = colTbls.keys();

  // Loop 1, filling color_indices
  for (QString table_ref: table_names) {
    QList<QString> color_names = colTbls[table_ref]->colors.keys();

    for (int j = 0; j < color_names.size(); j++)
      if (!color_indices.contains(color_names[j]))
          color_indices.insert(color_names[j], current_index++);
  }

  // Loop 2, filling colTbls
  for (ColorTable* tbl: colTbls) {
    std::vector<float> color_vec(3u * static_cast<uint>(color_indices.size()));

    for (QString col_name: color_indices.keys()) {
      uint index = color_indices[col_name];

      if (tbl->colors.contains(col_name)) {
        color_vec[3*index+0] = static_cast<float>(tbl->colors[col_name].redF());
        color_vec[3*index+1] = static_cast<float>(tbl->colors[col_name].greenF());
        color_vec[3*index+2] = static_cast<float>(tbl->colors[col_name].blueF());
      } else {
        color_vec[3*index+0] = 0;
        color_vec[3*index+1] = 0;
        color_vec[3*index+2] = 0;
      }
    }

    tbl->table = color_vec;
  }
}



void S52References::print(void) {

  qDebug() << "";
  qDebug() << "Lookups";
  qDebug() << "------------------------";
  qDebug() << "";
  for (auto mp: lookups)
    for (auto lps: mp)
      for (LookUp* lp: lps)
        qDebug() << lp->OBCL << lp->RCID;

  /*
  qDebug() << "";
  qDebug() << "Color Tables";
  qDebug() << "------------------------";
  qDebug() << "";
  for (int i = 0; i < colTbls.keys().size(); i++) {
    QString table_key =  colTbls.keys()[i];
    ColorTable* ct = colTbls[table_key];

    qDebug() << ct->name;
    qDebug() << ct->graphics_file;
    for (int j = 0; j < ct->colors.keys().size(); j++) {
      QString color_key =  ct->colors.keys()[j];
      QColor c = ct->colors[color_key];
      qDebug() << color_key << " (" << c.redF() << ", "
               << c.greenF() << ", " << c.blueF() << ")";
    }

    qDebug() << "";
  }

  qDebug() << "";
  qDebug() << "Lookups";
  qDebug() << "------------------------";
  qDebug() << "";
  for (int i = 0; i < lookups.keys().size(); i++) {
    int table_key =  lookups.keys()[i];
    LookUp* lp = lookups[table_key];

    qDebug() << lp->name;
    qDebug() << "id: " << lp->id;
    qDebug() << "rcid: " << lp->rcid;
    qDebug() << "type: " << lp->type;
    qDebug() << "disp_prio: " << lp->disp_prio;
    qDebug() << "radar_prio: " << lp->radar_prio;
    qDebug() << "table_name: " << lp->table_name;
    qDebug() << "instruction: " << lp->instructions;
    qDebug() << "display_cat: " << lp->display_cat;
    qDebug() << "comment: " << lp->comment;
    qDebug() << "attr_refs: " << lp->attr_refs;
    qDebug() << "";
  }

  qDebug() << "";
  qDebug() << "Line styles";
  qDebug() << "------------------------";
  qDebug() << "";
  for (int i = 0; i < line_styles.keys().size(); i++) {
    int table_key =  line_styles.keys()[i];
    LineStyle* ls = line_styles[table_key];

    qDebug() << ls->name;
    qDebug() << "rcid: " << ls->rcid;
    qDebug() << "description: " << ls->description;
    qDebug() << "color_ref: " << ls->color_ref;
    qDebug() << "vector size: " << ls->vector.size;
    qDebug() << "vector distance: " << ls->vector.distance;
    qDebug() << "vector origin: " << ls->vector.origin;
    qDebug() << "vector pivot: " << ls->vector.pivot;
    qDebug() << "vector hpgl: " << ls->vector.hpgl;

    qDebug() << "";
  }

  qDebug() << "";
  qDebug() << "Patterns";
  qDebug() << "------------------------";
  qDebug() << "";
  for (int i = 0; i < patterns.keys().size(); i++) {
    int table_key =  patterns.keys()[i];
    Pattern* pt = patterns[table_key];

    qDebug() << pt->name;
    qDebug() << "rcid: " << pt->rcid;
    qDebug() << "description: " << pt->description;
    qDebug() << "color_ref: " << pt->color_ref;
    qDebug() << "definition: " << pt->definition;
    qDebug() << "filltype: " << pt->filltype;
    qDebug() << "spacing: " << pt->spacing;
    qDebug() << "vector size: " << pt->vector.size;
    qDebug() << "vector distance: " << pt->vector.distance;
    qDebug() << "vector origin: " << pt->vector.origin;
    qDebug() << "vector pivot: " << pt->vector.pivot;
    qDebug() << "vector hpgl: " << pt->vector.hpgl;

    qDebug() << "";
  }

  qDebug() << "";
  qDebug() << "Symbols";
  qDebug() << "------------------------";
  qDebug() << "";
  for (int i = 0; i < symbols.keys().size(); i++) {
    QString table_key =  symbols.keys()[i];
    Symbol* sl = symbols[table_key];

    qDebug() << sl->name;
    qDebug() << "rcid: " << sl->rcid;
    qDebug() << "description: " << sl->description;
    qDebug() << "color_ref: " << sl->color_ref;
    qDebug() << "definition: " << sl->definition;

    qDebug() << "vector size: " << sl->vector.size;
    qDebug() << "vector distance: " << sl->vector.distance;
    qDebug() << "vector origin: " << sl->vector.origin;
    qDebug() << "vector pivot: " << sl->vector.pivot;
    qDebug() << "vector hpgl: " << sl->vector.hpgl;

    qDebug() << "bitmap size: " << sl->bitmap.size;
    qDebug() << "bitmap distance: " << sl->bitmap.distance;
    qDebug() << "bitmap origin: " << sl->bitmap.origin;
    qDebug() << "bitmap pivot: " << sl->bitmap.pivot;
    qDebug() << "bitmap graphics loc: " << sl->bitmap.graphics_location;

    qDebug() << "";
  }
  */
}


S52References::~S52References() {
  for (auto ct: colTbls)
    delete ct;

  for (auto mp: lookups)
    for (auto lps: mp.values())
      for (auto lp: lps)
        delete lp;
}


void S52References::setColorScheme(const QString& name) {
  if (colTbls.contains(name))
    _color_scheme = name;
}


void S52References::readColorTables(QXmlStreamReader* xml) {
  ColorTable* ct = nullptr;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "color-table") {
        ct = new ColorTable();
        ct->name = xml->attributes().value("name").toString();
        break;
      }

      if (ct != nullptr && xml->name() == "graphics-file")
        ct->graphics_file = xml->attributes().value("name").toString();

      if (ct != nullptr && xml->name() == "color")
        ct->colors.insert(xml->attributes().value("name").toString(),
                          QColor(xml->attributes().value("r").toInt()
                               , xml->attributes().value("g").toInt()
                               , xml->attributes().value("b").toInt()));

      break;
    case QXmlStreamReader::EndElement:
      if (ct != nullptr && xml->name() == "color-table")
        colTbls.insert(ct->name, ct);

      if (xml->name() == "color-tables")
        return;

      break;
    default:
      break;
    }
  }
}


void S52References::readLookUps(QXmlStreamReader* xml) {
  LookUp* lp = nullptr;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "lookup") {
        lp = new LookUp();
        lp->nSequence = xml->attributes().value("id").toInt();
        lp->RCID = xml->attributes().value("RCID").toInt();
        lp->OBCL = xml->attributes().value("name").toString();
        break;
      }

      if (lp != nullptr && xml->name() == "type")
        lp->FTYP = CHART_OBJ_TYPE_MAP.value(xml->readElementText(), ChartObjectType::CHART_OBJ_TYPE_COUNT);

      if (lp != nullptr && xml->name() == "disp-prio")
        lp->DPRI = CHART_DISP_PRIO_MAP.value(xml->readElementText(), ChartDispPrio::DISP_PRIO_NUM);

      if (lp != nullptr && xml->name() == "radar-prio")
        lp->RPRI = CHART_RADAR_PRIO_MAP.value(xml->readElementText(), ChartRadarPrio::RAD_PRIO_NUM);

      if (lp != nullptr && xml->name() == "table-name")
        lp->TNAM = LOOKUP_TYPE_MAP.value(xml->readElementText(), LookUpTable::LUP_TABLE_COUNT);

      if (lp != nullptr && xml->name() == "attrib-code") {
        int index = xml->attributes().value("index").toInt();
        lp->ALST.insert(index, xml->readElementText());
      }

      if (lp != nullptr && xml->name() == "instruction")
        lp->INST = xml->readElementText().split(";");

      if (lp != nullptr && xml->name() == "display-cat")
        lp->DISC = CHART_DISPLAY_CAT_MAP.value(xml->readElementText(), ChartDisplayCat::DISP_CAT_MARINERS_OTHER);

      if (lp != nullptr && xml->name() == "comment")
        lp->LUCM = xml->readElementText().toInt();

      break;
    case QXmlStreamReader::EndElement:
      if (lp != nullptr && xml->name() == "lookup") {
        if (!lookups.contains(lp->TNAM))
          lookups.insert(lp->TNAM, QMap<QString, QVector<LookUp*>>());

        if (lookups[lp->TNAM].contains(lp->OBCL))
          lookups[lp->TNAM][lp->OBCL].push_back(lp);
        else
          lookups[lp->TNAM].insert(lp->OBCL, QVector<LookUp*>{ lp });
      }

      if (xml->name() == "lookups")
        return;

      break;
    default:
      break;
    }
  }
}


void S52References::readLineStyles(QXmlStreamReader* xml) {
  LineStyle ls;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "line-style") {
        ls = LineStyle();
        ls.rcid = xml->attributes().value("RCID").toInt();
        break;
      }

      if (xml->name() == "name")
        ls.name = xml->readElementText();

      if (xml->name() == "description")
        ls.description = xml->readElementText();

      if (xml->name() == "color-ref")
        ls.color_ref = xml->readElementText();

      if (xml->name() == "vector")
        ls.vector.size = QVector2D(xml->attributes().value("width").toInt()
                          , xml->attributes().value("height").toInt());

      if (xml->name() == "distance")
        ls.vector.distance = QVector2D(xml->attributes().value("min").toInt()
                              , xml->attributes().value("max").toInt());

      if (xml->name() == "pivot")
        ls.vector.pivot = QVector2D(xml->attributes().value("x").toInt()
                           , xml->attributes().value("y").toInt());

      if (xml->name() == "origin")
        ls.vector.origin = QVector2D(xml->attributes().value("x").toInt()
                            , xml->attributes().value("y").toInt());

      if (xml->name() == "HPGL")
        ls.vector.hpgl = xml->readElementText();

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "line-style")
        line_styles.insert(ls.rcid, ls);

      if (xml->name() == "line-styles")
        return;

      break;
    default:
      break;
    }
  }
}

void S52References::readPatterns(QXmlStreamReader* xml) {
  Pattern pn;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "pattern") {
        pn = Pattern();
        pn.rcid = xml->attributes().value("RCID").toInt();
        break;
      }

      if (xml->name() == "name")
        pn.name = xml->readElementText();

      if (xml->name() == "definition")
        pn.definition = xml->readElementText();

      if (xml->name() == "filltype")
        pn.filltype = xml->readElementText();

      if (xml->name() == "spacing")
        pn.spacing = xml->readElementText();

      if (xml->name() == "description")
        pn.description = xml->readElementText();

      if (xml->name() == "color-ref")
        pn.color_ref = xml->readElementText();

      if (xml->name() == "vector")
        pn.vector.size = QVector2D(xml->attributes().value("width").toInt()
                          , xml->attributes().value("height").toInt());

      if (xml->name() == "distance")
        pn.vector.distance = QVector2D(xml->attributes().value("min").toInt()
                              , xml->attributes().value("max").toInt());

      if (xml->name() == "pivot")
        pn.vector.pivot = QVector2D(xml->attributes().value("x").toInt()
                           , xml->attributes().value("y").toInt());

      if (xml->name() == "origin")
        pn.vector.origin = QVector2D(xml->attributes().value("x").toInt()
                            , xml->attributes().value("y").toInt());

      if (xml->name() == "HPGL")
        pn.vector.hpgl = xml->readElementText();

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "pattern")
        patterns.insert(pn.rcid, pn);

      if (xml->name() == "patterns")
        return;

      break;
    default:
      break;
    }
  }
}

void S52References::readSymbols(QXmlStreamReader* xml) {
  Symbol sb;
  bool vector_part_flag = false;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement: {
      QStringRef name = xml->name();
      auto attrs = xml->attributes();

      if (name == "symbol") {
        sb = Symbol();
        sb.rcid = xml->attributes().value("RCID").toInt();break;
      }

      if (name == "name")
        sb.name = xml->readElementText();

      if (name == "definition")
        sb.definition = xml->readElementText();

      if (name == "description")
        sb.description = xml->readElementText();

      if (name == "color-ref")
        sb.color_ref = xml->readElementText();

      if (name == "vector") {
        sb.vector.size = QVector2D(attrs.value("width").toInt(), attrs.value("height").toInt());
        vector_part_flag = true;
      }

      if (name == "bitmap") {
        sb.bitmap.size = QSize(attrs.value("width").toInt(), attrs.value("height").toInt());
        vector_part_flag = false;
      }


      if (name == "distance") {
        if (vector_part_flag)
          sb.vector.distance = QVector2D(attrs.value("min").toInt(), attrs.value("max").toInt());
        else
          sb.bitmap.distance = QVector2D(attrs.value("min").toInt(), attrs.value("max").toInt());
      }

      if (name == "pivot") {
        if (vector_part_flag)
          sb.vector.pivot = QVector2D(attrs.value("x").toInt(), attrs.value("y").toInt());
        else
          sb.bitmap.pivot = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());
      }

      if (name == "origin") {
        if (vector_part_flag)
          sb.vector.origin = QVector2D( attrs.value("x").toInt(), attrs.value("y").toInt());
        else
          sb.bitmap.origin = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());
      }

      if (xml->name() == "HPGL")
        sb.vector.hpgl = xml->readElementText();

      if (xml->name() == "graphics-location")
        sb.bitmap.graphics_location = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());

      break;
    }
    case QXmlStreamReader::EndElement:
      if (xml->name() == "symbol") {
        symbols.insert(sb.name, sb);
        break;
      }

      if (xml->name() == "symbols")
        return;

      break;
    default:
      break;
    }
  }
}
