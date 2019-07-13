#ifndef S57CONDSYMB_H
#define S57CONDSYMB_H

#include <QString>
#include <ogrsf_frmts.h>
#include "s52references.h"

QString expandCondSymb( QString cs
                      , OGRFeature* obj
                      , OGRGeometry* geom
                      , LookUp* lp
                      , OGRDataSource* ds
                      , S52References* ref
                      , const QMap<QString, QVariant>& featAttrs
                      , double next_safe_cnt
                      , const QSet<int>& floatingATONArray
                      , const QSet<int>& rigidATONArray);

#endif // S57CONDSYMB_H
