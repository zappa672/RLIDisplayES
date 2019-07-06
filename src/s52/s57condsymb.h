#ifndef S57CONDSYMB_H
#define S57CONDSYMB_H

#include <QString>
#include <ogrsf_frmts.h>
#include "s52references.h"

QString expandCondSymb(QString cs, OGRFeature* obj, OGRwkbGeometryType geomType, LookUp* lp, OGRDataSource* ds);

#endif // S57CONDSYMB_H
