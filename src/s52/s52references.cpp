#include "s52references.h"

#include <qmath.h>

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

  /*
  QSet<QString> instrs;
  for (auto mp: lookups)
    for (auto lps: mp.values())
      for (auto lp: lps)
        for (auto i: lp->INST)
          instrs.insert(i);

  for (auto i: instrs)
    qDebug() << i; */

  //print();
}

LookUp S52References::findBestLookUp(const QString& name, const QMap<QString, QVariant>& objAttrs, LookUpTable tbl) {
  QVector<LookUp> lups = lookups[tbl].value(name, QVector<LookUp>());
  LookUp best;
  int countATT = 0;

  // Loop through the available lookups for the feature
  for (LookUp lup: lups) {
    //qDebug() << "";
    //qDebug() << "Check lookup candidate " << lup.OBCL << lup.RCID << lup.INST;
    //qDebug() << lup->ALST;

    countATT = 0;

    for (const QString& lupAttr: lup.ALST) {
      QString lupAttrName = lupAttr.left(6);
      QString lupAttrVal = lupAttr.right(lupAttr.length() - 6).trimmed();

      //qDebug() << "lupAttr" << lupAttrName << lupAttrVal;
      if (objAttrs.contains(lupAttrName)) {
        bool attValMatch = false;

        // special case (i)
        if (lupAttrVal.isEmpty()) {  //No value to check, go to next lup_attr
          ++countATT;
          //qDebug() << "attr existing match";
          continue;
        }

        // special case (ii)
        //TODO  Find an ENC with "UNKNOWN" DRVAL1 or DRVAL2 and debug this code
        // Match if the object does NOT contain this attribute
        if( lupAttrVal == QString("?") ) {  // if LUP attribute value is "undefined"
          //qDebug() << "undefined attrval";
          continue;
        }

        QVariant objAttrVal = objAttrs[lupAttr.left(6)];
        //qDebug() << objAttrVal;
        switch (objAttrVal.type()) {
          case QVariant::Int:
            if (lupAttrVal.toInt() == objAttrVal.toInt()) {
              //qDebug() << "int match";
              attValMatch = true;
            }
            break;
          case QVariant::Double:
            if(std::abs(lupAttrVal.toDouble() - objAttrVal.toDouble()) < 1e-6 ) {
              //qDebug() << "double match";
              attValMatch = true;
            }
            break;
          case QVariant::String:
            if (lupAttrVal.trimmed() == objAttrVal.toString().trimmed()) {
              //qDebug() << "string match";
              attValMatch = true;
            }
            break;
          case QVariant::List: {
            auto lupAttrLst = lupAttrVal.split(",");
            auto objAttrLst = objAttrVal.toList();

            if (lupAttrLst.size() == objAttrLst.size()) {
              for (int i = 0; i < lupAttrLst.size(); i++)
                if (lupAttrLst[i].toInt() != objAttrLst[i].toInt())
                  break;

              //qDebug() << "string match";
              attValMatch = true;
            }

            break;
          }
          default:
            break;
          }

        if (attValMatch)
          ++countATT;
        //else
        //  qDebug() << "attr not match";
      } else {
        //qDebug() << "attr not found";
      }
    }

    // According to S52 specs, match must be perfect,
    // and the first 100% match is selected
    if (countATT == lup.ALST.size()) { // Full match
      best = lup;
      //qDebug() << "match lookup: " << lup->RCID;
      break;
    }
  }

  //if (countATT != best->ALST.size())
  //  qDebug() << "lookup not found";

  return best;
}



void S52References::fillColorTables() {
  uint current_index = 0;
  QList<QString> table_names = _colTbls.keys();

  // Loop 1, filling _colorIndices
  for (QString table_ref: table_names) {
    QList<QString> color_names = _colTbls[table_ref]->colors.keys();

    for (int j = 0; j < color_names.size(); j++)
      if (!_colorIndices.contains(color_names[j]))
          _colorIndices.insert(color_names[j], current_index++);
  }

  // Loop 2, filling _colTbls
  for (ColorTable* tbl: _colTbls) {
    std::vector<float> color_vec(3u * static_cast<uint>(_colorIndices.size()));

    for (QString col_name: _colorIndices.keys()) {
      uint index = _colorIndices[col_name];

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
      for (LookUp lp: lps)
        qDebug() << lp.OBCL << lp.RCID;

  /*
  qDebug() << "";
  qDebug() << "Color Tables";
  qDebug() << "------------------------";
  qDebug() << "";
  for (int i = 0; i < _colTbls.keys().size(); i++) {
    QString table_key =  _colTbls.keys()[i];
    ColorTable* ct = _colTbls[table_key];

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
  for (auto ct: _colTbls)
    delete ct;
}


void S52References::setColorScheme(const QString& name) {
  if (_colTbls.contains(name))
    _colorScheme = name;
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
        _colTbls.insert(ct->name, ct);

      if (xml->name() == "color-tables")
        return;

      break;
    default:
      break;
    }
  }
}


void S52References::readLookUps(QXmlStreamReader* xml) {
  LookUp lp;

  while (!xml->atEnd()) {
    switch (xml->readNext()) {
    case QXmlStreamReader::StartElement:
      if (xml->name() == "lookup") {
        lp = LookUp();
        lp.nSequence = xml->attributes().value("id").toInt();
        lp.RCID = xml->attributes().value("RCID").toInt();
        lp.OBCL = xml->attributes().value("name").toString();
        break;
      }

      if (xml->name() == "type")
        lp.FTYP = CHART_OBJ_TYPE_MAP.value(xml->readElementText(), ChartObjectType::COUNT);

      if (xml->name() == "disp-prio")
        lp.DPRI = CHART_DISP_PRIO_MAP.value(xml->readElementText(), ChartDispPrio::NUM);

      if (xml->name() == "radar-prio")
        lp.RPRI = CHART_RADAR_PRIO_MAP.value(xml->readElementText(), ChartRadarPrio::NUM);

      if (xml->name() == "table-name")
        lp.TNAM = LOOKUP_TYPE_MAP.value(xml->readElementText(), LookUpTable::COUNT);

      if (xml->name() == "attrib-code") {
        int index = xml->attributes().value("index").toInt();
        lp.ALST.insert(index, xml->readElementText());
      }

      if (xml->name() == "instruction")
        lp.INST = xml->readElementText().split(";");

      if (xml->name() == "display-cat")
        lp.DISC = CHART_DISPLAY_CAT_MAP.value(xml->readElementText(), ChartDisplayCat::MARINERS_OTHER);

      if (xml->name() == "comment")
        lp.LUCM = xml->readElementText().toInt();

      break;
    case QXmlStreamReader::EndElement:
      if (xml->name() == "lookup") {
        if (!lookups.contains(lp.TNAM))
          lookups.insert(lp.TNAM, QMap<QString, QVector<LookUp>>());

        if (lookups[lp.TNAM].contains(lp.OBCL))
          lookups[lp.TNAM][lp.OBCL].push_back(lp);
        else
          lookups[lp.TNAM].insert(lp.OBCL, QVector<LookUp>{ lp });
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
    case QXmlStreamReader::StartElement: {
      QStringRef name = xml->name();
      auto attrs = xml->attributes();

      if (name == "line-style") {
        ls = LineStyle();
        ls.rcid = attrs.value("RCID").toInt();
        break;
      }

      if (name == "name")
        ls.name = xml->readElementText();

      if (name == "description")
        ls.description = xml->readElementText();

      if (name == "color-ref")
        ls.color_ref = xml->readElementText();

      if (name == "vector")
        ls.vector.size = QSize(attrs.value("width").toInt(), attrs.value("height").toInt());

      if (name == "distance")
        ls.vector.distance = QVector2D(attrs.value("min").toInt(), attrs.value("max").toInt());

      if (name == "pivot")
        ls.vector.pivot = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());

      if (name == "origin")
        ls.vector.origin = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());

      if (name == "HPGL")
        ls.vector.hpgl = xml->readElementText();

      break;
    }
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
    case QXmlStreamReader::StartElement: {
      QStringRef name = xml->name();
      auto attrs = xml->attributes();

      if (name == "pattern") {
        pn = Pattern();
        pn.rcid = attrs.value("RCID").toInt();
        break;
      }

      if (name == "name")
        pn.name = xml->readElementText();

      if (name == "definition")
        pn.definition = xml->readElementText();

      if (name == "filltype")
        pn.filltype = xml->readElementText();

      if (name == "spacing")
        pn.spacing = xml->readElementText();

      if (name == "description")
        pn.description = xml->readElementText();

      if (name == "color-ref")
        pn.color_ref = xml->readElementText();

      if (name == "vector")
        pn.vector.size = QSize(attrs.value("width").toInt(), attrs.value("height").toInt());

      if (name == "distance")
        pn.vector.distance = QVector2D(attrs.value("min").toInt(), attrs.value("max").toInt());

      if (name == "pivot")
        pn.vector.pivot = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());

      if (name == "origin")
        pn.vector.origin = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());

      if (name == "HPGL")
        pn.vector.hpgl = xml->readElementText();

      break;
    }
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
        sb.rcid = attrs.value("RCID").toInt();break;
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
        sb.vector.size = QSize(attrs.value("width").toInt(), attrs.value("height").toInt());
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
          sb.vector.pivot = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());
        else
          sb.bitmap.pivot = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());
      }

      if (name == "origin") {
        if (vector_part_flag)
          sb.vector.origin = QPoint(attrs.value("x").toInt(), attrs.value("y").toInt());
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

