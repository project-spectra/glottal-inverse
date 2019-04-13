#include "operators.h"
#include "operators_restore.h"

void setFilenames(string& raw, string& compressed, size_t mu) {

    std::ostringstream out;
    out << PERSIST_DIR << "/C_" << mu;
  
    raw = out.str();
    
    out << ".gz";
    compressed = out.str();

}
