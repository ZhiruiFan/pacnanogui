#ifndef PRENANO_H
#define PRENANO_H

namespace PRENANO {
/*  source type  */
const int USE_ORIGIN_FIELD = 0;
const int USE_MIRROR_FIELD = 1;

/*  visualization type  */
const bool USE_MODEL_MODE = true;
const bool USE_FIELD_MODE = false;

/*  hide mode or extract model in picking  */
const bool USE_HIDE_MODE    = true;
const bool USE_EXTRACT_MODE = false;

/*  cell mode or node mode  */
const bool USE_CELL_MODE = true;
const bool USE_NODE_MODE = false;

/*  field update mode  */
const bool FIELD_UPDATE   = false;
const bool FIELD_GENERATE = true;

}  // namespace PRENANO

#endif  // PRENANO_H
